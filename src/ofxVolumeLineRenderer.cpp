//
//  ofxVolumeLineRenderer.c
//  VolumeLine
//
//  Created by Nakazi_w0w on 2015/10/28.
//
//

#include "ofxVolumeLineRenderer.hpp"
#include "Shader.hpp"

ofxVolumeLineRenderer::ofxVolumeLineRenderer()
:_femiBuffer()
,_shader(0)
,_vao()
{
    _femiBuffer = std::make_shared<FemiBuffer<LinePoint> >(GL_ARRAY_BUFFER);
    
    _shader = glCreateProgram();
    Shader vs(_shader, GL_VERTEX_SHADER, "line_shader/vert.glsl");
    Shader gs(_shader, GL_GEOMETRY_SHADER, "line_shader/geom.glsl");
    Shader fs(_shader, GL_FRAGMENT_SHADER, "line_shader/frag_solid.glsl");
    
    if(vs.hasError()) {
        std::cout << "vs : " << vs.getError() << std::endl;
    }
    if(gs.hasError()) {
        std::cout << "gs : " << gs.getError() << std::endl;
    }
    if(fs.hasError()) {
        std::cout << "fs : " << fs.getError() << std::endl;
    }
    
    vs.attach(_shader);
    gs.attach(_shader);
    fs.attach(_shader);
    
    std::string linkLog;
    if(shader_link(_shader, &linkLog) == false) {
        std::cout << "link : " << linkLog << std::endl;
    }
    
    _vao = std::make_shared<VertexArrayObject>();
}
ofxVolumeLineRenderer::~ofxVolumeLineRenderer() {
    glDeleteProgram(_shader);
}

void ofxVolumeLineRenderer::reserve(int linePointCount) {
    _femiBuffer->reserve(linePointCount);
}

void ofxVolumeLineRenderer::update(std::function<void(std::vector<LinePoint> &)> store) {
    if(_femiBuffer->update(store)) {
        printf("[ofxVolumeLineRenderer] allocated\n");
    }
}

void ofxVolumeLineRenderer::draw(const glm::mat4& vmat, const glm::mat4& pmat, bool cullingFlip) {
    if(_femiBuffer->getUpdatedInstanceCount() == 0) {
        return;
    }

    glUseProgram(_shader);
    _vao->bind();
    
    glUniformMatrix4fv(glGetUniformLocation(_shader, "u_mvmat"), 1, GL_FALSE, glm::value_ptr(vmat));
    glUniformMatrix4fv(glGetUniformLocation(_shader, "u_pmat"), 1, GL_FALSE, glm::value_ptr(pmat));
    
    GLuint in_position_location = 0;
    GLuint in_color_location = 1;
    GLuint in_radius_location = 6;
    glEnableVertexAttribArray(in_position_location);
    glEnableVertexAttribArray(in_color_location);
    glEnableVertexAttribArray(in_radius_location);
    glBindBuffer(GL_ARRAY_BUFFER, _femiBuffer->getVbo());
    glVertexAttribPointer(in_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(LinePoint), (void *)offsetof(LinePoint, position));
    glVertexAttribPointer(in_color_location, 3, GL_FLOAT, GL_FALSE, sizeof(LinePoint), (void *)offsetof(LinePoint, color));
    glVertexAttribPointer(in_radius_location, 1, GL_FLOAT, GL_FALSE, sizeof(LinePoint), (void *)offsetof(LinePoint, radius));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glEnable(GL_CULL_FACE);
    if(cullingFlip) {
        glCullFace(GL_FRONT);
    } else {
        glCullFace(GL_BACK);
    }
    
    glDrawArrays(GL_LINES, 0, _femiBuffer->getUpdatedInstanceCount());
    
    glDisable(GL_CULL_FACE);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisableVertexAttribArray(in_radius_location);
    glDisableVertexAttribArray(in_color_location);
    glDisableVertexAttribArray(in_position_location);
    
    _vao->unbind();
    glUseProgram(0);
    
    _dummy.begin();
    _dummy.end();
}
void ofxVolumeLineRenderer::drawIsDone() {
    _femiBuffer->drawIsDone();
}