//
//  VertexBufferObject.h
//  emptyExample
//
//  Created by yoshimura on 6/25/15.
//
//

#pragma once


#include <OpenGL/gl3.h>
#include <iostream>
#include <cassert>

class VertexBufferObject {
public:
    // target => GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER
    // usage => GL_DYNAMIC_DRAW, GL_STATIC_DRAW
    VertexBufferObject(const uint8_t *bytes, std::size_t size, GLenum target, GLenum usage):_vbo(0), _target(target), _size(size){
        glGenBuffers(1, &_vbo);
        glBindBuffer(target, _vbo);
        glBufferData(target, size, bytes, usage);
        glBindBuffer(target, 0);
    }
    ~VertexBufferObject() {
        glDeleteBuffers(1, &_vbo);
    }
    
    void store(const uint8_t *bytes) {
        glBindBuffer(_target, _vbo);
        glBufferSubData(_target, 0, _size, bytes);
        glBindBuffer(_target, 0);
    }
    void storeUnSync(const uint8_t *bytes, int size) {
        assert(_size >= size);
        glBindBuffer(_target, _vbo);
        uint8_t *ptr = (uint8_t *)glMapBufferRange(_target, 0, _size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        memcpy(ptr, bytes, size);
        glUnmapBuffer(_target);
        glBindBuffer(_target, 0);
    }
    
    GLuint getVBO() {
        return _vbo;
    }
    std::size_t size() const {
        return _size;
    }
private:
    VertexBufferObject(const VertexBufferObject&);
    void operator=(const VertexBufferObject&);
private:
    GLuint _vbo;
    GLenum _target;
    std::size_t _size;
};