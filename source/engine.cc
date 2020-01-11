#include <iostream>
#include <stdexcept>
#include <functional>
#include <boost/filesystem.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include "engine.hh"

using namespace boost::filesystem;

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
    : chassis{chassis}, pr{(unsigned) chassis.get_width(), (unsigned) chassis.get_height()}, gui{*this} {
    glfwSetWindowUserPointer(chassis.get_window(), &ih);
    glfwSetKeyCallback(chassis.get_window(), &InputHandler::glfw_keyboard_handler);
    glfwSetCursorPosCallback(chassis.get_window(), &InputHandler::glfw_mouse_handler);
    glfwSetMouseButtonCallback(chassis.get_window(), &InputHandler::glfw_mouse_click_handler);

    update_view();

    ih.set_press_handler("engine_exploration", std::function<void(int)>(
        [this](int key) {
            if (key == GLFW_KEY_ESCAPE) {
                set_exploration_mode(!exploring);
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
            explo_mx -= last_mxpos - xpos;
            explo_my -= last_mypos - ypos;
            last_mxpos = xpos;
            last_mypos = ypos;
            cam.set_angles(explo_mx, explo_my);
            update_view();
        }
    ));

    ih.set_mouse_click_handler("obj_picking", std::function<void(int, int)>(
        [this](int button, int action) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                double xpos, ypos;
                glfwGetCursorPos(this->chassis.get_window(), &xpos, &ypos);

                auto obj_id = this->pr.get_mesh_id(xpos, ypos);
                if (obj_id.has_value()) {
                    std::cout << "Clicked on object: " << *obj_id << std::endl;
                } else {
                    std::cout << "Clicked on: " << xpos << ", " << ypos << std::endl;
                }
            }
        }
    ));

    ih.long_press_handler_enabled("mr_navigation", false);
    ih.mouse_handler_enabled("mr_navigation", false);

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
        
        if (!exploring) {
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
                    auto p_model = std::shared_ptr<TexturedMesh>(new TexturedMesh{attribs_data, indices_data, "res/models/" + std::string{path.data}});
                    tr.add_mesh(p_model);
                    pr.add_mesh(p_model);
                    p_models.push_back(p_model);
                }
            } else {
                auto p_model = std::shared_ptr<Mesh>(new Mesh{attribs_data, indices_data});
                cr.add_mesh(p_model);
                pr.add_mesh(p_model);
                p_models.push_back(p_model);
            }

        }
    } else {
        throw std::runtime_error("Failed to load model: " + path);
    }
}

void Engine::set_exploration_mode(bool exploration_mode) {
    exploring = exploration_mode;
    
    if (exploration_mode) {
        glfwGetCursorPos(chassis.get_window(), &last_mxpos, &last_mypos);
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

void Engine::update_view() {
    cr.update_view(cam);
    tr.update_view(cam);
    pr.update_view(cam);
}

void Engine::compute_click_pixels() {
    
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
