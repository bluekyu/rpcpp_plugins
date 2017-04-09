/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../include/flex_buffer.hpp"

FlexBuffer::FlexBuffer(NvFlexLibrary* lib): positions_(lib), velocities_(lib), phases_(lib), active_indices_(lib)
{
}

void FlexBuffer::destroy(void)
{
    positions_.destroy();
    velocities_.destroy();
    phases_.destroy();

    active_indices_.destroy();
}

void FlexBuffer::map(void)
{
    positions_.map();
    velocities_.map();
    phases_.map();

    active_indices_.map();
}

void FlexBuffer::unmap(void)
{
    positions_.unmap();
    velocities_.unmap();
    phases_.unmap();

    active_indices_.unmap();
}
