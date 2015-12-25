//
//  FemiBuffer.hpp
//  Hyundai
//
//  Created by Nakazi_w0w on 2015/10/27.
//
//

#pragma once

#include <vector>
#include <memory>
#include <type_traits>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ofMain.h"

#include "SyncGL.hpp"
#include "VertexBufferObject.hpp"
#include "VertexArrayObject.hpp"


template <class T>
class FemiBuffer {
public:
    static_assert(std::is_standard_layout<T>::value, "standard layout requied.");
    
    FemiBuffer(GLenum target)
    :_target(target)
    ,_updatedInstanceCount(0)
    ,_inflightIndex(0){
    }
    ~FemiBuffer() {
        for(int i = 0 ; i < kInflightBuffers ; ++i) {
            _instanceBufferSyncs[i].waitIfNeeded();
        }
    }
    
    FemiBuffer(const FemiBuffer&) = delete;
    void operator=(const FemiBuffer&) = delete;
    
    enum {
        kInflightBuffers = 3
    };
    
    void reserve(int instanceCount) {
        // 再アロケーション
        for(int i = 0 ; i < kInflightBuffers ; ++i) {
            _instanceBufferSyncs[i].waitIfNeeded();
            
            _instanceBuffers[i].reserve(instanceCount);
            _instanceBuffers_vbo[i] = std::make_shared<VertexBufferObject>(nullptr,
                                                                           sizeof(T) * instanceCount,
                                                                           _target,
                                                                           GL_DYNAMIC_DRAW);
        }
    }
    bool update(std::function<void(std::vector<T> &)> store) {
        // update index
        _inflightIndex = (_inflightIndex + 1) % kInflightBuffers;
        
        // sync
        _instanceBufferSyncs[_inflightIndex].waitIfNeeded();
        
        
        // memory
        _instanceBuffers[_inflightIndex].clear();
        store(_instanceBuffers[_inflightIndex]);
        
        _updatedInstanceCount = _instanceBuffers[_inflightIndex].size();
        
        //
        if(_updatedInstanceCount == 0) {
            return false;
        }
        
        bool isAllocated = false;
        int allocated_size = _instanceBuffers_vbo[_inflightIndex] ? _instanceBuffers_vbo[_inflightIndex]->size() : 0;
        
        int instanceSize = sizeof(T);
        int required_size = _updatedInstanceCount * instanceSize;
        if(allocated_size < required_size) {
            _instanceBuffers_vbo[_inflightIndex] = std::make_shared<VertexBufferObject>(nullptr,
                                                                                        required_size,
                                                                                        _target,
                                                                                        GL_DYNAMIC_DRAW);
            isAllocated = true;
        }

        _instanceBuffers_vbo[_inflightIndex]->storeUnSync((const uint8_t *)_instanceBuffers[_inflightIndex].data(),
                                                          sizeof(T) * _updatedInstanceCount);
        return isAllocated;
    }
    
    void drawIsDone() {
        _instanceBufferSyncs[_inflightIndex].addTask();
    }
    
    GLuint getVbo() {
        return _instanceBuffers_vbo[_inflightIndex]->getVBO();
    }
    int getUpdatedInstanceCount() {
        return _updatedInstanceCount;
    }
private:
    GLenum _target;
    
    int _inflightIndex;
    int _updatedInstanceCount;
    
    SyncGL _instanceBufferSyncs[kInflightBuffers];
    std::vector<T> _instanceBuffers[kInflightBuffers];
    std::shared_ptr<VertexBufferObject> _instanceBuffers_vbo[kInflightBuffers];
};