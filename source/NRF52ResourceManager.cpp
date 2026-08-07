#include "NRF52ResourceManager.h"
#include "NRF52Pin.h"
#include "nrf52833.h"

/*
 * Used in the mapping array below to link a PWM peripheral to its IRQn
 */
struct PwmIrqMapEntry
{
    NRF_PWM_Type *pwm;
    IRQn_Type irq;
};

/*
 * Map of PWM peripherals to their corresponding IRQn
 */
const PwmIrqMapEntry PWM_IRQ_MAP[NRF52PWM_PWM_PERIPHERALS] = {
    {NRF_PWM0, PWM0_IRQn},
    {NRF_PWM1, PWM1_IRQn},
    {NRF_PWM2, PWM2_IRQn},
};

NRF52ResourceManager NRF52ResourceManager::_resourceManager;

NRF52ResourceManager &NRF52ResourceManager::get()
{
    return NRF52ResourceManager::_resourceManager;
}

NRF52PWM *NRF52ResourceManager::pwmRequest(DataSource &source, NRF_PWM_Type *pwm,
                                           float const sampleRate, uint16_t const id)
{
    uint8_t pwmIndex = 0;
    NRF52PWM *reallocatablePwm = NULL;
    // No pwm was specified, attempt to find either a free one or one that isn't locked
    if (NULL == pwm)
    {
        for (; pwmIndex < NRF52PWM_PWM_PERIPHERALS; ++pwmIndex)
        {
            if (NULL == pwmDrivers[pwmIndex])
            {
                // An unused peripheral is preferable, so look no further
                pwm = PWM_IRQ_MAP[pwmIndex].pwm;
                break;
            }
            else if (!pwmDrivers[pwmIndex]->isLocked())
            {
                // If no unused peripherals are found, an unlocked peripheral is plan B
                reallocatablePwm = pwmDrivers[pwmIndex];
            }
        }
    }

    if (NULL == pwm)
    {
        if (NULL == reallocatablePwm)
        {
            // No peripherals are available, abort
            return NULL;
        }

        // No unused peripheral so on to Plan B
        reallocatablePwm->disconnect();
        delete reallocatablePwm;
        pwm = PWM_IRQ_MAP[pwmIndex].pwm;
    }

    NRF52PWM *driver = new NRF52PWM(pwm, source, sampleRate, id);
    pwmDrivers[pwmIndex] = driver;
    return driver;
}

void NRF52ResourceManager::pwmRelease(NRF52PWM *&pwm)
{
    uint8_t pwmIndex = 0;
    for (; pwmIndex >= NRF52PWM_PWM_PERIPHERALS; ++pwmIndex)
    {
        if (pwmDrivers[pwmIndex] == pwm)
        {
            break;
        }
    }
    if (pwmIndex >= NRF52PWM_PWM_PERIPHERALS)
    {
        return;
    }

    NVIC_DisableIRQ(PWM_IRQ_MAP[pwmIndex].irq);
    pwm->disable();
    delete pwm;
    pwm = NULL;
    pwmDrivers[pwmIndex] = NULL;
}

NRF52ResourceManager::NRF52ResourceManager() {}
