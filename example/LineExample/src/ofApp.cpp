#include "ofApp.h"

glm::vec3 lorenz(const glm::vec3& position, float p, float r, float b) {
    return glm::vec3(-p * position.x + p * position.y,
                     -position.x * position.z + r * position.x - position.y,
                     position.x * position.y - b * position.z);
}

//--------------------------------------------------------------
void ofApp::setup(){
    glEnable(GL_FRAMEBUFFER_SRGB);
    ofSetEscapeQuitsApp(false);
    ofSetVerticalSync(true);
    
    _renderer = std::make_shared<ofxVolumeLineRenderer>();

    _gui = new ofxPanel();
    _gui->setup();
    _gui->add(_p.setup("p", 10.0f, 0.0f, 20.0f));
    _gui->add(_r.setup("r", 28.0f, 0.0f, 100.0f));
    _gui->add(_b.setup("b", 8.0f/3.0f, 0.0f, 10.0f));
    
    _position = glm::vec3(10.0f, 0.0f, 0.0f);
}

//--------------------------------------------------------------
void ofApp::update(){
    float delta = 1.0f / 60.0f;
    float param_p = _p;
    float param_r = _r;
    float param_b = _b;
    
    int step = 5;
    for(int i = 0 ; i < step ; ++i) {
        float delta_step = delta / step;
        
        
        glm::vec3 velocity = lorenz(_position, param_p, param_r, param_b);
        _position = _position + velocity * delta_step;
        
        PositionHistory history;
        history.position = _position;
        history.velocity = glm::length(velocity);
        
        _linestrip.insert(_linestrip.begin(), history);
        if(_linestrip.size() > 10000) {
            _linestrip.pop_back();
        }
        
        // printf("%f\n", history.velocity);
    }
    
    auto map_color = [](float velocity) {
        float h = ofMap(velocity, 0.0f, 100.0f, 0.0f, 60.0f);
        return glm::rgbColor(glm::vec3(h, 1.0f, 1.0f));
    };
    
    auto map_radius = [](float velocity) {
        return ofMap(velocity, 0.0f, 100.0f, 0.2f, 1.0f);
    };

    _renderer->update([=](std::vector<ofxVolumeLineRenderer::LinePoint> &instances) {
        float scale = 5.0f;
        for(int i = 1 ; i < _linestrip.size() ; ++i) {
            PositionHistory src_p1 = _linestrip[i - 1];
            PositionHistory src_p2 = _linestrip[i];
            
            ofxVolumeLineRenderer::LinePoint p1;
            ofxVolumeLineRenderer::LinePoint p2;
            
            p1.position = src_p1.position * scale;
            p1.color = map_color(src_p1.velocity);
            p1.radius = map_radius(src_p1.velocity);
            
            p2.position = src_p2.position * scale;
            p2.color = map_color(src_p2.velocity);
            p2.radius = map_radius(src_p2.velocity);
            
            instances.push_back(p1);
            instances.push_back(p2);
        }
    });
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofClear(0);
    ofEnableDepthTest();
    
    _cam.begin();
    
    ofPushMatrix();
    ofRotateZ(90.0f);
    ofSetColor(255);
    ofDrawGridPlane(300.0f);
    
    ofPopMatrix();
    
    ofDrawAxis(500);
    
    glm::mat4 projmat;
    memcpy(glm::value_ptr(projmat), ofGetCurrentMatrix(OF_MATRIX_PROJECTION).getPtr(), sizeof(glm::mat4));
    glm::mat4 viewmat;
    memcpy(glm::value_ptr(viewmat), ofGetCurrentMatrix(OF_MATRIX_MODELVIEW).getPtr(), sizeof(glm::mat4));
    
    _renderer->draw(viewmat, projmat, false);
    
    _renderer->drawIsDone();
    
    _cam.end();
    
    ofDisableDepthTest();
    
    _gui->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
