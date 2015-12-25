//
//  SyncGL.hpp
//  emptyExample
//
//  Created by yoshimura on 7/7/15.
//
//

#pragma once

#include <cassert>
#include <OpenGL/gl3.h>
#include <map>

class SyncGL {
public:
    SyncGL():_hasTask(false), _sync(0){
    }
    bool hasTask() const {
        return _hasTask;
    }
    
    void addTask() {
        assert(_hasTask == false);
        _sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        _hasTask = true;
    }
    
    void waitTask() {
        assert(_hasTask);
        
        int ms = 10 * 1000;  /* 10 sec*/
        GLenum r = glClientWaitSync(_sync, 0, 1000 * 1000 * ms);
        if(r == GL_TIMEOUT_EXPIRED || r == GL_WAIT_FAILED) {
            static std::map<GLenum, const char *> to_s = {
                {GL_ALREADY_SIGNALED, "GL_ALREADY_SIGNALED"},
                {GL_TIMEOUT_EXPIRED, "GL_TIMEOUT_EXPIRED"},
                {GL_CONDITION_SATISFIED, "GL_CONDITION_SATISFIED"},
                {GL_WAIT_FAILED, "GL_WAIT_FAILED"},
            };
            printf("sync error: %s\n", to_s[r]);
        }
        if(r == GL_TIMEOUT_EXPIRED) {
            // no wait
        }
        if(r == GL_CONDITION_SATISFIED) {
            // wait
        }
        
        glDeleteSync(_sync);
        _sync = 0;
        _hasTask = false;
    }
    
    void waitIfNeeded() {
        if(this->hasTask()) {
            this->waitTask();
        }
    }
private:
    SyncGL(const SyncGL&);
    void operator=(const SyncGL&);
private:
    bool _hasTask;
    GLsync _sync;
};
