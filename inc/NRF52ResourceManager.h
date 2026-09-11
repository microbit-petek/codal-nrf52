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

/**
 * This is the method by which a resource is obtained. The resource type and instance are specified,
 * followed by the parameters required to instantiate the resource driver. For example:
 *
 * NRF52PWM * pwm0 = GET_RESOURCE(PWM, 0, *dataSource, *consumer, 44100);
 *
 * So long as the resource is not locked by another consumer, the resource will be configured and
 * a pointer to the driver will be returned. Otherwise, the pointer will be NULL.
 */
#define GET_RESOURCE(resourceType, instanceNumber, ...)                                            \
    ({                                                                                             \
        NRF52##resourceType *resource = NULL;                                                      \
        resource_manager::NRF52ResourceManager &resourceManager =                                  \
            resource_manager::NRF52ResourceManager::_get();                                        \
        if (DEVICE_BUSY != resourceManager.releaseResource(resourceType##instanceNumber))          \
        {                                                                                          \
            resource = new NRF52##resourceType(NRF_##resourceType##instanceNumber, __VA_ARGS__);   \
            resourceManager.registerResource(resourceType##instanceNumber, *resource);             \
        }                                                                                          \
        resource;                                                                                  \
    })

#define RELEASE_RESOURCE(resource)                                                                 \
    resource_manager::NRF52ResourceManager::_get().releaseResource(resource);                      \
    resource = NULL;

enum ResourceId
{
    PWM0,
    PWM1,
    PWM2,
    PWM3,
    RESOURCE_COUNT
};

namespace resource_manager
{

class NRF52ResourceManager : public CodalComponent
{
public:
    /*
     * The resource manager keeps track of which resources are in use, allowing for dynamic
     * reassignment. The resource manager (and this function) are not designed to be called
     * directly, instead use the GET_RESOURCE() macro to facilitate aquisition of resources.
     *
     * @return reference to the Resource Manager instance
     */
    static NRF52ResourceManager &_get();

    /**
     * Releases a resource based on its ID. This is used by the GET_RESOURCE() macro to signal that
     * a resource has been requested. The resource manager acts as a lookup, matching the ID to the
     * current instance of the resource's driver (if it exists) and facilitates the disconnection if
     * the resource is not locked.
     *
     * @param[in] id The ID of the resource to be released
     *
     * @return DEVICE_OK if the resource is now free
     *         DEVICE_BUSY if the resource is locked
     *         DEVICE_INVALID_PARAMETER if the resource was linked to the wrong consumer
     */
    ErrorCode releaseResource(ResourceId);

    /**
     * Releases a resource based on its reference. This is used when the releasing of a resource is
     * triggered by its consumer. This function could be combined with the one above if the Resource
     * type was modified to hold its ID, passed in on construction. However, this would mean that
     * all resource constructors would need another ID field, which didn't seem like a good
     * solution.
     *
     * @param[in] resource Reference to the resource being released
     *
     * @return DEVICE_OK if the resource is now free
     *         DEVICE_INVALID_PARAMETER if the resource does not appear in the resource manager's
     * table
     */
    ErrorCode releaseResource(Resource *);

    /**
     * Registers a resource with the resource manager.
     *
     * @param[in] id The ID of the resource being registered
     * @param[in] resource Reference to the resource being registered
     *
     * @return DEVICE_OK if the resource was successfully registered
     *         DEVICE_INVALID_STATE if a resource is already registered to that ID
     */
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
} // namespace resource_manager

#endif // #ifndef NRF52_RESOURCE_MANAGER
