//
//  ofxVolumeLineRenderer.hpp
//  VolumeLine
//
//  Created by Nakazi_w0w on 2015/10/28.
//
//

#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ofMain.h"

#include "SyncGL.hpp"
#include "VertexBufferObject.hpp"
#include "VertexArrayObject.hpp"

#include "FemiBuffer.hpp"

class ofxVolumeLineRenderer {
public:
    ofxVolumeLineRenderer();
    ~ofxVolumeLineRenderer();
    
    ofxVolumeLineRenderer(const ofxVolumeLineRenderer&) = delete;
    void operator=(const ofxVolumeLineRenderer&) = delete;
    
    struct LinePoint {
        glm::vec3 position;
        glm::vec3 color;
        float radius = 0.06f;
    };
    
    // allocate
    void reserve(int linePointCount);
    
    // update lines. if instanceCount is too many, extend buffer.
    void update(std::function<void(std::vector<LinePoint> &)> store);
    
    // drawing
    void draw(const glm::mat4& vmat, const glm::mat4& pmat, bool cullingFlip);
    void drawIsDone();
    
    int getUpdatedInstanceCount() {
        return _femiBuffer->getUpdatedInstanceCount();
    }
private:
    std::shared_ptr<FemiBuffer<LinePoint>> _femiBuffer;
    GLuint _shader;
    std::shared_ptr<VertexArrayObject> _vao;
    
    ofShader _dummy;
};
