#pragma once
#ifndef FB_CLASS_H
#define FB_CLASS_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <string>
class Framebuffer
    //To use bind();
    //To deactivate unbind();
{
public:
	unsigned int ID;
	unsigned int textureID;
	string type;
    int W, H;
    Framebuffer(int texWidth, int texHeight, const std::string& typeFB)
    {
        type = typeFB;
        W = texWidth;
        H = texHeight;
        // Create framebuffer
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        // Create texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        if (typeFB == "color") {
            // Color framebuffer
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        }
        else if (typeFB == "depth") {
            // Depth-only framebuffer
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texWidth, texHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);

            // No color buffer for depth-only FBO
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else {
            std::cerr << "❌ Unknown framebuffer type: " << typeFB << std::endl;
        }

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "❌ Framebuffer is not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

	void bind() {
        glViewport(0, 0, W, H);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
        glClear(GL_DEPTH_BUFFER_BIT);
    };
	void unbind(int sc_width = -1, int sc_height = -1) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(sc_width != -1 && sc_height != -1) glViewport(0, 0, sc_width, sc_height);

    };
    void bindTexture(int unit ){
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    void unbindTexture(int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    };
};

#endif