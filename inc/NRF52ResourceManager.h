/*
The MIT License (MIT)

Copyright (c) 2026 Lancaster University & Micro:bit Educational Foundation.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef NRF52_RESOURCE_MANAGER
#define NRF52_RESOURCE_MANAGER

#include "ErrorNo.h"
#include "NRF52PWM.h"
#include "Resource.h"

#define GET_RESOURCE(resourceType, instanceNumber, ...)                                            \
    ({                                                                                             \
        NRF52##resourceType *resource = NULL;                                                      \
        NRF52ResourceManager &resourceManager = NRF52ResourceManager::get();                       \
        if (DEVICE_BUSY != resourceManager.releaseResource(resourceType##instanceNumber))          \
        {                                                                                          \
            resource = new NRF52##resourceType(NRF_##resourceType##instanceNumber, __VA_ARGS__);   \
            resourceManager.registerResource(resourceType##instanceNumber, *resource);             \
        }                                                                                          \
        resource;                                                                                  \
    })

enum ResourceId
{
    PWM0,
    PWM1,
    PWM2,
    PWM3,
    RESOURCE_COUNT
};

namespace codal
{

class NRF52ResourceManager : public CodalComponent
{
public:
    /*
     * The Resource Manager manages access to the hardware peripherals, making sure that each
     * peripheral is only being used by one component at a time, and allowing dynamic reallocation
     * of peripherals when they're no longer required.
     * The resource manager exists as a static variable.
     * This function provides access to it through a reference.
     *
     * @return reference to the Resource Manager instance
     */
    static NRF52ResourceManager &get();

    ErrorCode releaseResource(ResourceId);

    ErrorCode releaseResource(Resource &);

    ErrorCode registerResource(ResourceId, Resource &);

private:
    /*
     * Private constructor: The resource manager is instantiated as a static and accessed through
     * the public `resourceManager()` function
     */
    NRF52ResourceManager();

    static NRF52ResourceManager _resourceManager;

    Resource *resourceTable[RESOURCE_COUNT];
};
} // namespace codal

#endif // #ifndef NRF52_RESOURCE_MANAGER
