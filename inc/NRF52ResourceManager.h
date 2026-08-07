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

#include "NRF52PWM.h"

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

    /*
     * Request a PWM driver from the Resource Manager
     *
     * @param[in] source     The DataSource which will provide data to the PWM
     * @param[in] pwm        (Optional) The desired pwm peripheral. If not specified, the resource
     *                       manager will attempt to get the next available peripheral.
     * @param[in] sampleRate The frequency (in Hz) at which data will be sampled
     * @param[in] id         Device id used to identify events from this device on the message bus
     *
     * @return Either: - Pointer to a driver for the requested peripheral if successfully acquired
     *                 - NULL if no peripheral could be acquired
     */
    NRF52PWM *pwmRequest(DataSource &source, NRF_PWM_Type *pwm = NULL,
                         float const sampleRate = NRF52PWM_DEFAULT_FREQUENCY,
                         uint16_t const id = DEVICE_ID_SYSTEM_DAC);

    /*
     * Release a PWM driver back to the Resource Manager to be reused.
     * This function will set `pwm` to NULL so the driver can no longer be used.
     * If an invalid pointer is passed this function will do nothing.
     *
     * @param[in,out] pwm The driver to be released
     *
     * @return Enum representing the result of the release request
     */
    void pwmRelease(NRF52PWM *&pwm);

private:
    /*
     * Private constructor: The resource manager is instantiated as a static and accessed through
     * the public `resourceManager()` function
     */
    NRF52ResourceManager();

    static NRF52ResourceManager _resourceManager;

    NRF52PWM *pwmDrivers[NRF52PWM_PWM_PERIPHERALS] = {NULL};
};
} // namespace codal

#endif // #ifndef NRF52_RESOURCE_MANAGER
