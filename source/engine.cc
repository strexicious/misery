#include <iostream>
#include <stdexcept>
#include <functional>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include "engine.hh"

using namespace std::filesystem;

bool Engine::engine_started = false;
std::vector<std::string> Engine::model_names{};

void load_model_names(std::vector<std::string>& v) {
    if (v.empty()) {
        directory_iterator di("res/models/");
        // empty argument constructor means end
        while (di != directory_iterator()) {
            if (di->path().extension() == ".obj") {
                v.push_back(di->path().filename().string());
            }
            ++di;
        }
    }
}

Engine::Engine(Chassis& chassis)
    : chassis{chassis}, pr{chassis.get_width(), chassis.get_height(), p_models}, gui{*this},
    fru{chassis.get_width(), chassis.get_height()} {
    glfwSetWindowUserPointer(chassis.get_window(), &ih);
    glfwSetKeyCallback(chassis.get_window(), &InputHandler::glfw_keyboard_handler);
    glfwSetCursorPosCallback(chassis.get_window(), &InputHandler::glfw_mouse_handler);
    glfwSetMouseButtonCallback(chassis.get_window(), &InputHandler::glfw_mouse_click_handler);

    update_projection();
    update_view();

    ih.set_press_handler("engine_exploration", std::function<void(int)>(
        [this](int key) {
            if (key == GLFW_KEY_ESCAPE) {
                set_exploration_mode(!exploring);
                
                if (moving_object.has_value()) {
                    ih.mouse_click_handler_enabled("obj_move_commit", false);
                    picked = false;
                    ih.mouse_handler_enabled("obj_moving", false);
                    Mesh& obj = moving_object.value();
                    obj.translate(this->last_pos - obj.cur_translate(), 1.0f);
                    moving_object = std::nullopt;
                }
            }
        }
    ));

    ih.set_long_press_handler("mr_navigation", std::function<void(int)>(
        [this](int key) {
            if (key == GLFW_KEY_W) {
                cam.move(Camera::MoveDir::forward);
                update_view();
            }

            if (key == GLFW_KEY_S) {
                cam.move(Camera::MoveDir::backward);
                update_view();
            }
        }
    ));

    ih.set_mouse_handler("mr_navigation", std::function<void(double, double)>(
        [this](double xpos, double ypos) {
            this->explo_pos.cur_mx -= this->explo_pos.last_mxpos - xpos;
            this->explo_pos.cur_my -= this->explo_pos.last_mypos - ypos;
            this->explo_pos.last_mxpos = xpos;
            this->explo_pos.last_mypos = ypos;
            cam.set_angles(this->explo_pos.cur_mx, this->explo_pos.cur_my);
            update_view();
        }
    ));

    ih.set_mouse_handler("obj_moving", std::function<void(double, double)>(
        [this](double xpos, double ypos) {
            // normalize x,y movement so it ranges [-1, 1]
            // x needs to be multiplied to 1.77, because it doesn't really range between [-1,1]
            // because of the aspect ratio correction
            double moved_x = fru.get_aspect_ratio() * 2.0f * (xpos - this->moving_obj_pos.last_mxpos) / this->chassis.get_width();
            double moved_y = 2.0f * (this->moving_obj_pos.last_mypos - ypos) / this->chassis.get_height();
            auto cam_space_trans = glm::vec3(moved_x, moved_y, 0.0f);

            // since the object is only translated,
            // translation vectors in both world space and object space are same.
            auto trans_matrix = this->cam.to_world_space(cam_space_trans);

            this->moving_object.value().get().translate(trans_matrix, -fru.get_orig_w(pixel_depth));
            this->moving_obj_pos.last_mxpos = xpos;
            this->moving_obj_pos.last_mypos = ypos;
        }
    ));

    ih.set_mouse_click_handler("obj_move_commit", std::function<void(int, int)>(
        [this](int button, int action) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                if (!picked && this->moving_object.has_value()) {
                    picked = true;
                } else if (picked) {
                    ih.mouse_click_handler_enabled("obj_move_commit", false);
                    picked = false;
                    ih.mouse_click_handler_enabled("obj_picking", true);
                    ih.mouse_handler_enabled("obj_moving", false);
                    moving_object = std::nullopt;
                }
            }

            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                ih.mouse_click_handler_enabled("obj_move_commit", false);
                picked = false;
                ih.mouse_click_handler_enabled("obj_picking", true);
                ih.mouse_handler_enabled("obj_moving", false);
                Mesh& obj = moving_object.value();
                obj.translate(this->last_pos - obj.cur_translate(), 1.0f);
                moving_object = std::nullopt;
            }
        }
    ));

    ih.set_mouse_click_handler("obj_picking", std::function<void(int, int)>(
        [this](int button, int action) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(this->chassis.get_window(), &xpos, &ypos);

                auto obj_info = this->pr.get_mesh_info(xpos, ypos);
                if (obj_info.has_value()) {
                    this->last_pos = this->p_models[obj_info->first]->cur_translate();
                    this->moving_object = *(this->p_models[obj_info->first]);
                    this->pixel_depth = obj_info->second;

                    glfwGetCursorPos(this->chassis.get_window(), &this->moving_obj_pos.last_mxpos, &this->moving_obj_pos.last_mypos);
                    
                    ih.mouse_click_handler_enabled("obj_move_commit", true);
                    ih.mouse_click_handler_enabled("obj_picking", false);
                    ih.mouse_handler_enabled("obj_moving", true);
                }
            }
        }
    ));

    ih.long_press_handler_enabled("mr_navigation", false);
    ih.mouse_handler_enabled("mr_navigation", false);
    ih.mouse_handler_enabled("obj_moving", false);
    ih.mouse_click_handler_enabled("obj_move_commit", false);

    load_model_names(model_names);
}

void Engine::run_loop() {
    double last_time = glfwGetTime();
    delta_time = last_time;
    while (!glfwWindowShouldClose(chassis.get_window())) {
        double now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;

        chassis.reset_clear_color();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        cr.render();
        tr.render();
        
        if (!exploring && !moving_object.has_value()) {
            pr.active_fbo();
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            pr.render();
            pr.active_default_fbo();
            
            gui.render();
        }

        glfwSwapBuffers(chassis.get_window());
        glfwPollEvents();
    }
}

void Engine::load_model(std::string const& path) {    
    std::string warn;
    std::string err;
    
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs
    );

    if (scene) {
        std::vector<GLfloat> attribs_data;
        std::vector<GLuint> indices_data;

        for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh* pMesh = scene->mMeshes[i];
            
            // should check for pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0
            // check if has a material (aka texture)
            // AI_SCENE_FLAGS_INCOMPLETE which rejects the scene if it doesn't have materials
            // if not set, assimp will fill the first, index 0, material by default, so we don't want that
            bool has_texture = pMesh->mMaterialIndex < scene->mNumMaterials && pMesh->mMaterialIndex != 0;
            
            if (has_texture) {
                attribs_data.resize(pMesh->mNumVertices * 5);
                for (unsigned j = 0; j < pMesh->mNumVertices; ++j) {
                    attribs_data[5*j+0] = pMesh->mVertices[j].x;
                    attribs_data[5*j+1] = pMesh->mVertices[j].y;
                    attribs_data[5*j+2] = pMesh->mVertices[j].z;
                    
                    attribs_data[5*j+3] = pMesh->mTextureCoords[0][j].x;
                    attribs_data[5*j+4] = pMesh->mTextureCoords[0][j].y;
                }
            } else {
                attribs_data.resize(pMesh->mNumVertices * 3);
                for (unsigned j = 0; j < pMesh->mNumVertices; ++j) {
                    attribs_data[3*j+0] = pMesh->mVertices[j].x;
                    attribs_data[3*j+1] = pMesh->mVertices[j].y;
                    attribs_data[3*j+2] = pMesh->mVertices[j].z;
                }
            }

            indices_data.resize(pMesh->mNumFaces * 3);
            for (unsigned j = 0; j < pMesh->mNumFaces; ++j) {
                indices_data[3*j+0] = pMesh->mFaces[j].mIndices[0];
                indices_data[3*j+1] = pMesh->mFaces[j].mIndices[1];
                indices_data[3*j+2] = pMesh->mFaces[j].mIndices[2];
            }
            
            if (has_texture) {
                aiMaterial* pMaterial = scene->mMaterials[pMesh->mMaterialIndex];

                aiString path;
                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
                    std::unique_ptr<Mesh> p_model = std::make_unique<TexturedMesh>(attribs_data, indices_data, "res/models/" + std::string{path.data});
                    p_models.push_back(std::move(p_model));
                    tr.add_mesh_idx(p_models.size() - 1);
                    pr.add_mesh_idx(p_models.size() - 1);
                }
            } else {
                std::unique_ptr<Mesh> p_model = std::make_unique<Mesh>(attribs_data, indices_data);
                p_models.push_back(std::move(p_model));
                cr.add_mesh_idx(p_models.size() - 1);
                pr.add_mesh_idx(p_models.size() - 1);
            }

        }
    } else {
        throw std::runtime_error("Failed to load model: " + path);
    }
}

void Engine::set_exploration_mode(bool exploration_mode) {
    exploring = exploration_mode;
    
    if (exploration_mode) {
        glfwGetCursorPos(chassis.get_window(), &explo_pos.last_mxpos, &explo_pos.last_mypos);
        glfwSetInputMode(chassis.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        ih.long_press_handler_enabled("mr_navigation", true);
        ih.mouse_handler_enabled("mr_navigation", true);
        ih.mouse_click_handler_enabled("obj_picking", false);
    } else {
        glfwSetInputMode(chassis.get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);        

        ih.long_press_handler_enabled("mr_navigation", false);
        ih.mouse_handler_enabled("mr_navigation", false);
        ih.mouse_click_handler_enabled("obj_picking", true);
    }
}

void Engine::update_projection() {
    cr.update_projection(fru);
    tr.update_projection(fru);
    pr.update_projection(fru);
}

void Engine::update_view() {
    cr.update_view(cam);
    tr.update_view(cam);
    pr.update_view(cam);
}

Engine::Gui::Gui(Engine& ngn)
    : ngn{ngn} { }

void Engine::Gui::render() {
    render_model_info();
}

void Engine::Gui::render_model_info() {
    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Models");
    {
        ImGui::ListBoxHeader("Models");
        for (unsigned i = 0; i < model_names.size(); ++i) {
            if (ImGui::Selectable(model_names[i].c_str(), selected == static_cast<int>(i))) {
                selected = static_cast<int>(i);
            }
        }
        ImGui::ListBoxFooter();

        if (selected != -1 && ImGui::Button("Load Model")) {
            try {
                ngn.load_model("res/models/" + model_names[selected]);
            } catch (std::runtime_error& e) {
                load_info = e.what();
            }
        }
        
        if (!load_info.empty()) {
            ImGui::Text(load_info.data());
        }
    }
    ImGui::End();

    ImGui::Begin("Debug Info");
    {
        ImGui::Text(("FPS: " + std::to_string(1/ngn.delta_time)).data());
    }
    ImGui::End();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
