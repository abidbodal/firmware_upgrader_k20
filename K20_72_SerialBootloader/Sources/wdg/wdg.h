/******************************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved                       
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file      wdg.h
*
* @author    b01119
* 
* @version   0.0.1.0
* 
* @date      May-18-2012
* 
* @brief     Watch dog simple driver header file
*
******************************************************************************/
#ifndef   _WDG_H
#define   _WDG_H



#define WDG_ResetMCU() WDOG_UNLOCK = 0xd928;  WDOG_UNLOCK = 0xc520;

#define WDG_Disable() WDOG_UNLOCK = 0xC520;  WDOG_UNLOCK = 0xD928; WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;               /* Disable watchdog */ 
#define WDG_Enable() WDOG_UNLOCK = 0xC520;  WDOG_UNLOCK = 0xD928; WDOG_STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;               /* Enable watchdog */ 
#define WDG_Refresh() WDOG_REFRESH = 0xA602; WDOG_REFRESH = 0xB480;

#endif





