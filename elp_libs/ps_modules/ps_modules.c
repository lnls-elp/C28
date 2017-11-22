/******************************************************************************
 * Copyright (C) 2017 by LNLS - Brazilian Synchrotron Light Laboratory
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. LNLS and
 * the Brazilian Center for Research in Energy and Materials (CNPEM) are not
 * liable for any misuse of this material.
 *
 *****************************************************************************/

/**
 * @file ps_modules.c
 * @brief Power supplies modules.
 * 
 * Main source file for power supply modules. It includes macros and enumerates
 * related to operation of power supplies from ELP group on Sirius Project.
 *
 * @author gabriel.brunheira
 * @date 25/10/2017
 *
 */

#include "ps_modules.h"
#include "../IPC_modules/IPC_modules.h"

/**
 * TODO: Put here your defines. Just what is local. If you don't
 * need to access it from other module, consider use a constant (const)
 */
#define PASSWORD    0xCAFE

/**
 * TODO: Put here your constants and variables. Always use static for 
 * private members.
 */

/**
 * TODO: Put here your function prototypes for private functions. Use
 * static in declaration.
 */

/**
 * Initialization of power supply module. It requires address of specific power
 * supply functions ```turn_on```, ```turn_off``` and ```reset_interlocks```.
 *
 * @param p_ps_module pointer to the ps module struct
 * @param model power supply model to be initialized
 * @param turn_on address of ```turn_on()``` function to that power supply
 * @param turn_off address of ```turn_off()``` function
 * @param isr_softinterlock address of ```isr_softinterlock()``` function
 * @param isr_hardinterlock address of ```isr_hardinterlock()``` function
 * @param reset_interlocks address of ```reset_interlocks()``` function to that power supply
 */
void init_ps_module(ps_module_t *p_ps_module, ps_model_t model,
                    void (*turn_on)(void), void (*turn_off)(void),
                    void (*isr_softinterlock)(void),
                    void (*isr_hardinterlock)(void),
                    void (*reset_interlocks)(void))
{
    p_ps_module->ps_status.bit.state        = Off;
    p_ps_module->ps_status.bit.openloop     = tOPEN_LOOP;
    p_ps_module->ps_status.bit.interface    = Remote;
    p_ps_module->ps_status.bit.active       = ACTIVE;
    p_ps_module->ps_status.bit.model        = model;
    p_ps_module->ps_status.bit.unlocked     = LOCKED;
    p_ps_module->ps_status.bit.reserved     = 0;

    p_ps_module->ps_setpoint        = 0.0;
    p_ps_module->ps_reference       = 0.0;

    p_ps_module->turn_on            = turn_on;
    p_ps_module->turn_off           = turn_off;
    p_ps_module->isr_softinterlock  = isr_softinterlock;
    p_ps_module->isr_hardinterlock  = isr_hardinterlock;
    p_ps_module->reset_interlocks   = reset_interlocks;
}

/**
 * Configuration of operation mode. All possible values of ps_state_t will be
 * implemented, but it's recommended to avoid using *Off*, *Interlock* and
 * *Initializing*, which are states reached by commands or events.
 *
 * @param p_ps_module pointer to the ps module struct
 * @param op_mode operation mode
 */
void cfg_ps_operation_mode(ps_module_t *p_ps_module, ps_state_t op_mode)
{
    switch(op_mode)
    {
        case Off:
        {
            p_ps_module->ps_setpoint = 0.0;
            p_ps_module->ps_reference = 0.0;
            p_ps_module->turn_off();
            break;
        }

        case Interlock:
        {
            p_ps_module->set_softinterlock();
            p_ps_module->ps_setpoint = 0.0;
            p_ps_module->ps_reference = 0.0;
            /// TODO:
            break;
        }

        case Initializing:
        {
            /// TODO:
            break;

        }
#if 0
        case SlowRef:
        {
            /// TODO:
            break;
        }
#endif
        case SlowRefSync:
        {
            /// TODO:
            break;
        }
#if 0
        case FastRef:
        {
            break;
        }
#endif
        case RmpWfm:
        {
            // TODO:
            IPC_CtoM_Msg.WfmRef = IPC_MtoC_Msg.WfmRef;
            IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferK = IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferEnd + 1;
            break;
        }

        case MigWfm:
        {
            // TODO:
            IPC_CtoM_Msg.WfmRef = IPC_MtoC_Msg.WfmRef;
            IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferK = IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferEnd + 1;

            break;
        }

        case Cycle:
        {
            disable_siggen(&p_ps_module->siggen);
            cfg_siggen(&p_ps_module->siggen, IPC_MtoC_Msg.SigGen.Type,
                       IPC_MtoC_Msg.SigGen.Ncycles, IPC_MtoC_Msg.SigGen.Freq,
                       p_ps_module->siggen.aux_param);
            break;
        }

        default:
        {
            return;
        }
    }

    p_ps_module->ps_status.bit.state = op_mode;
}

/**
 * Open control loop. From now on, ps_reference is treated as duty cycle value,
 * in percentage units [%].
 *
 * @param p_ps_module pointer to the ps module struct
 */
void open_loop(ps_module_t *p_ps_module)
{
    if( (p_ps_module->ps_status.bit.state == Off) ||
        (p_ps_module->ps_status.bit.unlocked == UNLOCKED) )
    {
        p_ps_module->ps_status.bit.openloop = tOPEN_LOOP;
    }
}

/**
 * Close control loop. From now on, ps_reference is treated as input reference
 * for load main control loop [A/V].
 *
 * @param p_ps_module pointer to the ps module struct
 */
void close_loop(ps_module_t *p_ps_module)
{
    if( (p_ps_module->ps_status.bit.state == Off) ||
        (p_ps_module->ps_status.bit.unlocked == UNLOCKED) )
    {
        p_ps_module->ps_status.bit.openloop = tCLOSED_LOOP;
    }
}

/**
 * Configuration of communication interface.
 *
 * @param p_ps_module pointer to the ps module struct
 * @param interface type of interface to be configured
 */
void cfg_ps_inteface(ps_module_t *p_ps_module, ps_interface_t interface)
{
    p_ps_module->ps_status.bit.interface = interface;
}

/**
 * Activate ps module. This bit is used on ps modules which controls more than
 * one independent power supply (e.g., FBP's).
 *
 * @param p_ps_module pointer to the ps module struct
 */
void activate_ps_module(ps_module_t *p_ps_module)
{
    if(p_ps_module->ps_status.bit.unlocked == UNLOCKED)
    {
        p_ps_module->ps_status.bit.active = ACTIVE;
    }
}

/**
 * Deactivate ps module. This bit is used on ps modules which controls more
 * than one independent power supply (e.g., FBP's).
 *
 * @param p_ps_module pointer to the ps module struct
 */
void deactivate_ps_module(ps_module_t *p_ps_module)
{
    if(p_ps_module->ps_status.bit.unlocked == UNLOCKED)
    {
        p_ps_module->ps_status.bit.active = INACTIVE;
    }
}

/**
 * Lock power supply. This prevents access to parameters from unauthorized
 * users.
 *
 * @param p_ps_module pointer to the ps module struct
 */
void lock_ps_module(ps_module_t *p_ps_module)
{
    p_ps_module->ps_status.bit.unlocked = LOCKED;
}

/**
 * Unlock power supply. This enables access to locked parameters.
 *
 * @param p_ps_module pointer to the ps module struct
 * @param password
 */
void unlock_ps_module(ps_module_t *p_ps_module, uint16_t password)
{
    if(password == PASSWORD)
    {
        p_ps_module->ps_status.bit.unlocked = UNLOCKED;
    }
}

/**
 * Return power supply model. This can be used to check if the correct ps
 * module is running.
 *
 * @param p_ps_module p_ps_module pointer to the ps module struct
 * @return power supply model
 */
ps_model_t get_ps_model(ps_module_t *p_ps_module)
{
    return (ps_model_t) p_ps_module->ps_status.bit.model;
}