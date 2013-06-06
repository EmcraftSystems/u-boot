/*******************************************************************************
 * (c) Copyright 2012 Microsemi Corporation.  All rights reserved.
 *
 *  Configure ZL30363 device.
 *
 * SVN $Revision: $
 * SVN $Date: $
 */

#include <common.h>
#include "mss_spi/mss_spi.h"

/*==============================================================================
 * ZL30362 configuration.
 * The values for this configuration were created using the ZL30362 GUI.
 */
const uint8_t g_zl30362_config[] =
{
    // ClockCenter Register Configuration File
    // Exported from GUI Ver. 2.1.0, 11:39:15, Nov 26, 2012
    // Product family ZL30362
    0x9F,        // Register address 0x0: Ready indicator
    0x3E,        // Register address 0x1: ID_Register
    0x02,        // Register address 0x2: HW Revision
    0x00,        // Register address 0x3: Reserved
    0x00,        // Register address 0x4: Reserved
    0x05,        // Register address 0x5: Reserved
    0xF6,        // Register address 0x6: Reserved
    0xFF,        // Register address 0x7: Customer identification [31:24]
    0xFF,        // Register address 0x8: Customer identification [23:16]
    0xFF,        // Register address 0x9: Customer identification [15:8]
    0xFF,        // Register address 0xA: Customer identification [7:0]
    0x04,        // Register address 0xB: Central Freq Offset [31:24]
    0x6A,        // Register address 0xC: Central Freq Offset [23:16]
    0xAA,        // Register address 0xD: Central Freq Offset [15:8]
    0xAB,        // Register address 0xE: Central Freq Offset [7:0]
    0x00,        // Register address 0xF: Reserved
    0x00,        // Register address 0x10: APLL Frequency Offset
    0x00,        // Register address 0x11: Sticky Lock Register
    0x00,        // Register address 0x12: Reserved
    0x00,        // Register address 0x13: Reserved
    0x00,        // Register address 0x14: Reserved
    0x00,        // Register address 0x15: Reserved
    0x00,        // Register address 0x16: Reserved
    0x00,        // Register address 0x17: Reserved
    0x01,        // Register address 0x18: Reset Status
    0xF7,        // Register address 0x19: GPIO at Startup [15:8]
    0x00,        // Register address 0x1A: GPIO at Startup [7:0]
    0x00,        // Register address 0x1B: Reserved
    0x00,        // Register address 0x1C: Reserved
    0x00,        // Register address 0x1D: Reserved
    0x00,        // Register address 0x1E: Reserved
    0x00,        // Register address 0x1F: Reserved
    0x00,        // Register address 0x20: Reference Fail 7-0
    0x00,        // Register address 0x21: Reference Fail 10-8
    0x00,        // Register address 0x22: DPLL Status
    0x00,        // Register address 0x23: Ref Fail Mask 7-0
    0x00,        // Register address 0x24: Ref Fail Mask 10-8
    0x00,        // Register address 0x25: DPLL Fail Mask
    0x1E,        // Register address 0x26: Reference Monitor Fail 0
    0x1E,        // Register address 0x27: Reference Monitor Fail 1
    0x1E,        // Register address 0x28: Reference Monitor Fail 2
    0x1E,        // Register address 0x29: Reference Monitor Fail 3
    0x1E,        // Register address 0x2A: Reference Monitor Fail 4
    0x1E,        // Register address 0x2B: Reference Monitor Fail 5
    0x1E,        // Register address 0x2C: Reference Monitor Fail 6
    0x1E,        // Register address 0x2D: Reference Monitor Fail 7
    0x1E,        // Register address 0x2E: Reference Monitor Fail 8
    0x1E,        // Register address 0x2F: Reference Monitor Fail 9
    0x1E,        // Register address 0x30: Reference Monitor Fail 10
    0x00,        // Register address 0x31: Reserved
    0x00,        // Register address 0x32: Reserved
    0x00,        // Register address 0x33: Reserved
    0x00,        // Register address 0x34: Reserved
    0x00,        // Register address 0x35: Reserved
    0x16,        // Register address 0x36: Reference Monitor Mask 0
    0x16,        // Register address 0x37: Reference Monitor Mask 1
    0x16,        // Register address 0x38: Reference Monitor Mask 2
    0x16,        // Register address 0x39: Reference Monitor Mask 3
    0x16,        // Register address 0x3A: Reference Monitor Mask 4
    0x16,        // Register address 0x3B: Reference Monitor Mask 5
    0x16,        // Register address 0x3C: Reference Monitor Mask 6
    0x16,        // Register address 0x3D: Reference Monitor Mask 7
    0x16,        // Register address 0x3E: Reference Monitor Mask 8
    0x16,        // Register address 0x3F: Reference Monitor Mask 9
    0x16,        // Register address 0x40: Reference Monitor Mask 10
    0x00,        // Register address 0x41: Reserved
    0x00,        // Register address 0x42: Reserved
    0x00,        // Register address 0x43: Reserved
    0x00,        // Register address 0x44: Reserved
    0x00,        // Register address 0x45: Reserved
    0xAA,        // Register address 0x46: GST Disqualify Time 3-0
    0xAA,        // Register address 0x47: GST Disqualify Time 7-4
    0x2A,        // Register address 0x48: GST Disqualify Time 10-8
    0x00,        // Register address 0x49: Reserved
    0x55,        // Register address 0x4A: GST Qualify Time 3-0
    0x55,        // Register address 0x4B: GST Qualify Time 7-4
    0x15,        // Register address 0x4C: GST Qualify Time 10-8
    0x00,        // Register address 0x4D: Reserved
    0x00,        // Register address 0x4E: Reserved
    0x00,        // Register address 0x4F: Reserved
    0x55,        // Register address 0x50: SCM/CFM Limit 0
    0x55,        // Register address 0x51: SCM/CFM Limit 1
    0x55,        // Register address 0x52: SCM/CFM Limit 2
    0x55,        // Register address 0x53: SCM/CFM Limit 3
    0x55,        // Register address 0x54: SCM/CFM Limit 4
    0x55,        // Register address 0x55: SCM/CFM Limit 5
    0x55,        // Register address 0x56: SCM/CFM Limit 6
    0x55,        // Register address 0x57: SCM/CFM Limit 7
    0x55,        // Register address 0x58: SCM/CFM Limit 8
    0x55,        // Register address 0x59: SCM/CFM Limit 9
    0x55,        // Register address 0x5A: SCM/CFM Limit 10
    0x00,        // Register address 0x5B: Reserved
    0x00,        // Register address 0x5C: Reserved
    0x00,        // Register address 0x5D: Reserved
    0x00,        // Register address 0x5E: Reserved
    0x00,        // Register address 0x5F: Reserved
    0x33,        // Register address 0x60: PFM Limit 1-0
    0x33,        // Register address 0x61: PFM Limit 3-2
    0x33,        // Register address 0x62: PFM Limit 5-4
    0x33,        // Register address 0x63: PFM Limit 7-6
    0x33,        // Register address 0x64: PFM Limit 9-8
    0x03,        // Register address 0x65: PFM Limit 10
    0x00,        // Register address 0x66: Reserved
    0x00,        // Register address 0x67: Reserved
    0xFF,        // Register address 0x68: Phase Acquisition Enable 7-0
    0x07,        // Register address 0x69: Phase Acquisition Enable 10-8
    0x0A,        // Register address 0x6A: Phase Memory Limit 0
    0x0A,        // Register address 0x6B: Phase Memory Limit 1
    0x0A,        // Register address 0x6C: Phase Memory Limit 2
    0x0A,        // Register address 0x6D: Phase Memory Limit 3
    0x0A,        // Register address 0x6E: Phase Memory Limit 4
    0x0A,        // Register address 0x6F: Phase Memory Limit 5
    0x0A,        // Register address 0x70: Phase Memory Limit 6
    0x0A,        // Register address 0x71: Phase Memory Limit 7
    0x0A,        // Register address 0x72: Phase Memory Limit 8
    0x0A,        // Register address 0x73: Phase Memory Limit 9
    0x0A,        // Register address 0x74: Phase Memory Limit 10
    0x00,        // Register address 0x75: Reserved
    0x00,        // Register address 0x76: Reserved
    0x00,        // Register address 0x77: Reserved
    0x00,        // Register address 0x78: Reserved
    0x00,        // Register address 0x79: Reserved
    0x00,        // Register address 0x7A: Reference Config 7-0
    0x00,        // Register address 0x7B: Reference Config 8
    0x00,        // Register address 0x7C: Reference Pre-divide 7-0
    0x00,        // Register address 0x7D: Reference Pre-divide 10-8
    0x00,        // Register address 0x7E: Microport Status
    0x01,        // Register address 0x7F: Page Select
    0x9C,        // Register address 0x80: Ref0 Base Freq Br [15:8]
    0x40,        // Register address 0x81: Ref0 Base Freq Br [7:0]
    0x0F,        // Register address 0x82: Ref0 Frequency Multiple Kr [15:8]
    0x30,        // Register address 0x83: Ref0 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0x84: Ref0 Numerator Mr [15:8]
    0x01,        // Register address 0x85: Ref0 Numerator Mr [7:0]
    0x00,        // Register address 0x86: Ref0 Denominator Nr [15:8]
    0x01,        // Register address 0x87: Ref0 Denominator Nr [7:0]
    0x9C,        // Register address 0x88: Ref1 Base Freq Br [15:8]
    0x40,        // Register address 0x89: Ref1 Base Freq Br [7:0]
    0x01,        // Register address 0x8A: Ref1 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0x8B: Ref1 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0x8C: Ref1 Numerator Mr [15:8]
    0x01,        // Register address 0x8D: Ref1 Numerator Mr [7:0]
    0x00,        // Register address 0x8E: Ref1 Denominator Nr [15:8]
    0x01,        // Register address 0x8F: Ref1 Denominator Nr [7:0]
    0x9C,        // Register address 0x90: Ref2 Base Freq Br [15:8]
    0x40,        // Register address 0x91: Ref2 Base Freq Br [7:0]
    0x01,        // Register address 0x92: Ref2 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0x93: Ref2 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0x94: Ref2 Numerator Mr [15:8]
    0x01,        // Register address 0x95: Ref2 Numerator Mr [7:0]
    0x00,        // Register address 0x96: Ref2 Denominator Nr [15:8]
    0x01,        // Register address 0x97: Ref2 Denominator Nr [7:0]
    0x9C,        // Register address 0x98: Ref3 Base Freq Br [15:8]
    0x40,        // Register address 0x99: Ref3 Base Freq Br [7:0]
    0x01,        // Register address 0x9A: Ref3 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0x9B: Ref3 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0x9C: Ref3 Numerator Mr [15:8]
    0x01,        // Register address 0x9D: Ref3 Numerator Mr [7:0]
    0x00,        // Register address 0x9E: Ref3 Denominator Nr [15:8]
    0x01,        // Register address 0x9F: Ref3 Denominator Nr [7:0]
    0x9C,        // Register address 0xA0: Ref4 Base Freq Br [15:8]
    0x40,        // Register address 0xA1: Ref4 Base Freq Br [7:0]
    0x01,        // Register address 0xA2: Ref4 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xA3: Ref4 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xA4: Ref4 Numerator Mr [15:8]
    0x01,        // Register address 0xA5: Ref4 Numerator Mr [7:0]
    0x00,        // Register address 0xA6: Ref4 Denominator Nr [15:8]
    0x01,        // Register address 0xA7: Ref4 Denominator Nr [7:0]
    0x9C,        // Register address 0xA8: Ref5 Base Freq Br [15:8]
    0x40,        // Register address 0xA9: Ref5 Base Freq Br [7:0]
    0x01,        // Register address 0xAA: Ref5 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xAB: Ref5 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xAC: Ref5 Numerator Mr [15:8]
    0x01,        // Register address 0xAD: Ref5 Numerator Mr [7:0]
    0x00,        // Register address 0xAE: Ref5 Denominator Nr [15:8]
    0x01,        // Register address 0xAF: Ref5 Denominator Nr [7:0]
    0x9C,        // Register address 0xB0: Ref6 Base Freq Br [15:8]
    0x40,        // Register address 0xB1: Ref6 Base Freq Br [7:0]
    0x01,        // Register address 0xB2: Ref6 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xB3: Ref6 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xB4: Ref6 Numerator Mr [15:8]
    0x01,        // Register address 0xB5: Ref6 Numerator Mr [7:0]
    0x00,        // Register address 0xB6: Ref6 Denominator Nr [15:8]
    0x01,        // Register address 0xB7: Ref6 Denominator Nr [7:0]
    0x9C,        // Register address 0xB8: Ref7 Base Freq Br [15:8]
    0x40,        // Register address 0xB9: Ref7 Base Freq Br [7:0]
    0x01,        // Register address 0xBA: Ref7 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xBB: Ref7 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xBC: Ref7 Numerator Mr [15:8]
    0x01,        // Register address 0xBD: Ref7 Numerator Mr [7:0]
    0x00,        // Register address 0xBE: Ref7 Denominator Nr [15:8]
    0x01,        // Register address 0xBF: Ref7 Denominator Nr [7:0]
    0x9C,        // Register address 0xC0: Ref8 Base Freq Br [15:8]
    0x40,        // Register address 0xC1: Ref8 Base Freq Br [7:0]
    0x01,        // Register address 0xC2: Ref8 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xC3: Ref8 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xC4: Ref8 Numerator Mr [15:8]
    0x01,        // Register address 0xC5: Ref8 Numerator Mr [7:0]
    0x00,        // Register address 0xC6: Ref8 Denominator Nr [15:8]
    0x01,        // Register address 0xC7: Ref8 Denominator Nr [7:0]
    0x9C,        // Register address 0xC8: Ref9 Base Freq Br [15:8]
    0x40,        // Register address 0xC9: Ref9 Base Freq Br [7:0]
    0x01,        // Register address 0xCA: Ref9 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xCB: Ref9 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xCC: Ref9 Numerator Mr [15:8]
    0x01,        // Register address 0xCD: Ref9 Numerator Mr [7:0]
    0x00,        // Register address 0xCE: Ref9 Denominator Nr [15:8]
    0x01,        // Register address 0xCF: Ref9 Denominator Nr [7:0]
    0x9C,        // Register address 0xD0: Ref10 Base Freq Br [15:8]
    0x40,        // Register address 0xD1: Ref10 Base Freq Br [7:0]
    0x01,        // Register address 0xD2: Ref10 Frequency Multiple Kr [15:8]
    0xE6,        // Register address 0xD3: Ref10 Frequency Multiple Kr [7:0]
    0x00,        // Register address 0xD4: Ref10 Numerator Mr [15:8]
    0x01,        // Register address 0xD5: Ref10 Numerator Mr [7:0]
    0x00,        // Register address 0xD6: Ref10 Denominator Nr [15:8]
    0x01,        // Register address 0xD7: Ref10 Denominator Nr [7:0]
    0x00,        // Register address 0xD8: Reserved
    0x00,        // Register address 0xD9: Reserved
    0x00,        // Register address 0xDA: Reserved
    0x00,        // Register address 0xDB: Reserved
    0x00,        // Register address 0xDC: Reserved
    0x00,        // Register address 0xDD: Reserved
    0x00,        // Register address 0xDE: Reserved
    0x00,        // Register address 0xDF: Reserved
    0x00,        // Register address 0xE0: Reserved
    0x00,        // Register address 0xE1: Reserved
    0x00,        // Register address 0xE2: Reserved
    0x00,        // Register address 0xE3: Reserved
    0x00,        // Register address 0xE4: Reserved
    0x00,        // Register address 0xE5: Reserved
    0x00,        // Register address 0xE6: Reserved
    0x00,        // Register address 0xE7: Reserved
    0x00,        // Register address 0xE8: Reserved
    0x00,        // Register address 0xE9: Reserved
    0x00,        // Register address 0xEA: Reserved
    0x00,        // Register address 0xEB: Reserved
    0x10,        // Register address 0xEC: Clock Sync Pulse 1-0
    0x32,        // Register address 0xED: Clock Sync Pulse 3-2
    0x54,        // Register address 0xEE: Clock Sync Pulse 5-4
    0x76,        // Register address 0xEF: Clock Sync Pulse 7-6
    0x98,        // Register address 0xF0: Clock Sync Pulse 9-8
    0x0A,        // Register address 0xF1: Clock Sync Pulse 10
    0x00,        // Register address 0xF2: Reserved
    0x00,        // Register address 0xF3: Reserved
    0x00,        // Register address 0xF4: Reserved
    0x00,        // Register address 0xF5: Reserved
    0x00,        // Register address 0xF6: Reserved
    0x00,        // Register address 0xF7: Reserved
    0x00,        // Register address 0xF8: Reserved
    0x00,        // Register address 0xF9: Reserved
    0x00,        // Register address 0xFA: Reserved
    0x00,        // Register address 0xFB: Reserved
    0x00,        // Register address 0xFC: Reserved
    0x00,        // Register address 0xFD: Reserved
    0x00,        // Register address 0xFE: Reserved
    0x02,        // Register address 0xFF: Page Select
    0x0C,        // Register address 0x100: DPLL0 Control
    0x00,        // Register address 0x101: DPLL0 Bandwidth Select
    0x00,        // Register address 0x102: DPLL0 Pull-in/Hold-in Select
    0x00,        // Register address 0x103: DPLL0 Mode/Reference Select
    0x00,        // Register address 0x104: DPLL0 Reference Select Status
    0x10,        // Register address 0x105: DPLL0 Ref Priority 1-0
    0x32,        // Register address 0x106: DPLL0 Ref Priority 3-2
    0x54,        // Register address 0x107: DPLL0 Ref Priority 5-4
    0x76,        // Register address 0x108: DPLL0 Ref Priority 7-6
    0x98,        // Register address 0x109: DPLL0 Ref Priority 9-8
    0x0A,        // Register address 0x10A: DPLL0 Ref Priority 10
    0x00,        // Register address 0x10B: Reserved
    0x00,        // Register address 0x10C: Reserved
    0x87,        // Register address 0x10D: DPLL0 Ref Fail Mask
    0x01,        // Register address 0x10E: DPLL0 PFM Fail Mask
    0x0B,        // Register address 0x10F: DPLL0 Holdover Delay/Edge Sel
    0x00,        // Register address 0x110: DPLL0 Phase Buildout Control
    0x23,        // Register address 0x111: DPLL0 PBO Phase Error Threshold
    0x70,        // Register address 0x112: DPLL0 PBO Min Phase Slope
    0x20,        // Register address 0x113: DPLL0 PBO End Interval
    0x64,        // Register address 0x114: DPLL0 PBO Timeout
    0x00,        // Register address 0x115: DPLL0 PBO Counter
    0x00,        // Register address 0x116: DPLL0 PBO Error Count [23:16]
    0x00,        // Register address 0x117: DPLL0 PBO Error Count [15:8]
    0x00,        // Register address 0x118: DPLL0 PBO Error Count [7:0]
    0x05,        // Register address 0x119: Reserved
    0x05,        // Register address 0x11A: Reserved
    0x00,        // Register address 0x11B: Reserved
    0x00,        // Register address 0x11C: Reserved
    0x00,        // Register address 0x11D: Reserved
    0x00,        // Register address 0x11E: Reserved
    0x00,        // Register address 0x11F: Reserved
    0x0C,        // Register address 0x120: DPLL1 Control
    0x00,        // Register address 0x121: DPLL1 Bandwidth Select
    0x00,        // Register address 0x122: DPLL1 Pull-in/Hold-in Select
    0x03,        // Register address 0x123: DPLL1 Mode/Reference Select
    0x00,        // Register address 0x124: DPLL1 Reference Select Status
    0x10,        // Register address 0x125: DPLL1 Ref Priority 1-0
    0x32,        // Register address 0x126: DPLL1 Ref Priority 3-2
    0x54,        // Register address 0x127: DPLL1 Ref Priority 5-4
    0x76,        // Register address 0x128: DPLL1 Ref Priority 7-6
    0x98,        // Register address 0x129: DPLL1 Ref Priority 9-8
    0x0A,        // Register address 0x12A: DPLL1 Ref Priority 10
    0x00,        // Register address 0x12B: Reserved
    0x00,        // Register address 0x12C: Reserved
    0x87,        // Register address 0x12D: DPLL1 Ref Fail Mask
    0x01,        // Register address 0x12E: DPLL1 PFM Fail Mask
    0x0B,        // Register address 0x12F: DPLL1 Holdover Delay/Edge Sel
    0x00,        // Register address 0x130: DPLL1 Phase Buildout Control
    0x23,        // Register address 0x131: DPLL1 PBO Jitter Threshold
    0x70,        // Register address 0x132: DPLL1 PBO Min Phase Slope
    0x20,        // Register address 0x133: DPLL1 PBO End Interval
    0x64,        // Register address 0x134: DPLL1 PBO Timeout
    0x00,        // Register address 0x135: DPLL1 PBO Counter
    0x00,        // Register address 0x136: DPLL1 PBO Error Count [23:16]
    0x00,        // Register address 0x137: DPLL1 PBO Error Count [15:8]
    0x00,        // Register address 0x138: DPLL1 PBO Error Count [7:0]
    0x05,        // Register address 0x139: Reserved
    0x05,        // Register address 0x13A: Reserved
    0x00,        // Register address 0x13B: Reserved
    0x00,        // Register address 0x13C: Reserved
    0x00,        // Register address 0x13D: Reserved
    0x00,        // Register address 0x13E: Reserved
    0x00,        // Register address 0x13F: Reserved
    0x0C,        // Register address 0x140: DPLL2 Control
    0x00,        // Register address 0x141: DPLL2 Bandwidth Select
    0x00,        // Register address 0x142: DPLL2 Pull-in/Hold-in Select
    0x03,        // Register address 0x143: DPLL2 Mode/Reference Select
    0x00,        // Register address 0x144: DPLL2 Reference Select Status
    0x10,        // Register address 0x145: DPLL2 Ref Priority 1-0
    0x32,        // Register address 0x146: DPLL2 Ref Priority 3-2
    0x54,        // Register address 0x147: DPLL2 Ref Priority 5-4
    0x76,        // Register address 0x148: DPLL2 Ref Priority 7-6
    0x98,        // Register address 0x149: DPLL2 Ref Priority 9-8
    0x0A,        // Register address 0x14A: DPLL2 Ref Priority 10
    0x00,        // Register address 0x14B: Reserved
    0x00,        // Register address 0x14C: Reserved
    0x87,        // Register address 0x14D: DPLL2 Ref Fail Mask
    0x01,        // Register address 0x14E: DPLL2 PFM Fail Mask
    0x0B,        // Register address 0x14F: DPLL2 Holdover Delay/Edge Sel
    0x00,        // Register address 0x150: DPLL2 Phase Buildout Control
    0x23,        // Register address 0x151: DPLL2 PBO Jitter Threshold
    0x70,        // Register address 0x152: DPLL2 PBO Min Phase Slope
    0x20,        // Register address 0x153: DPLL2 PBO End Interval
    0x64,        // Register address 0x154: DPLL2 PBO Timeout
    0x00,        // Register address 0x155: DPLL2 PBO Counter
    0x00,        // Register address 0x156: DPLL2 PBO Error Count [23:16]
    0x00,        // Register address 0x157: DPLL2 PBO Error Count [15:8]
    0x00,        // Register address 0x158: DPLL2 PBO Error Count [7:0]
    0x05,        // Register address 0x159: Reserved
    0x05,        // Register address 0x15A: Reserved
    0x00,        // Register address 0x15B: Reserved
    0x00,        // Register address 0x15C: Reserved
    0x00,        // Register address 0x15D: Reserved
    0x00,        // Register address 0x15E: Reserved
    0x00,        // Register address 0x15F: Reserved
    0x0C,        // Register address 0x160: DPLL3 Control
    0x00,        // Register address 0x161: DPLL3 Bandwidth Select
    0x00,        // Register address 0x162: DPLL3 Pull-in/Hold-in Select
    0x00,        // Register address 0x163: DPLL3 Mode/Reference Select
    0x00,        // Register address 0x164: DPLL3 Reference Select Status
    0x10,        // Register address 0x165: DPLL3 Ref Priority 1-0
    0x32,        // Register address 0x166: DPLL3 Ref Priority 3-2
    0x54,        // Register address 0x167: DPLL3 Ref Priority 5-4
    0x76,        // Register address 0x168: DPLL3 Ref Priority 7-6
    0x98,        // Register address 0x169: DPLL3 Ref Priority 9-8
    0x0A,        // Register address 0x16A: DPLL3 Ref Priority 10
    0x00,        // Register address 0x16B: Reserved
    0x00,        // Register address 0x16C: Reserved
    0x87,        // Register address 0x16D: DPLL3 Ref Fail Mask
    0x01,        // Register address 0x16E: DPLL3 PFM Fail Mask
    0x0B,        // Register address 0x16F: DPLL3 Holdover Delay/Edge Sel
    0x00,        // Register address 0x170: DPLL3 Phase Buildout Control
    0x23,        // Register address 0x171: DPLL3 PBO Jitter Threshold
    0x70,        // Register address 0x172: DPLL3 PBO Min Phase Slope
    0x20,        // Register address 0x173: DPLL3 PBO End Interval
    0x64,        // Register address 0x174: DPLL3 PBO Timeout
    0x00,        // Register address 0x175: DPLL3 PBO Counter
    0x00,        // Register address 0x176: DPLL3 PBO Error Count [23:16]
    0x00,        // Register address 0x177: DPLL3 PBO Error Count [15:8]
    0x00,        // Register address 0x178: DPLL3 PBO Error Count [7:0]
    0x05,        // Register address 0x179: Reserved
    0x05,        // Register address 0x17A: Reserved
    0x00,        // Register address 0x17B: Reserved
    0x00,        // Register address 0x17C: Reserved
    0x00,        // Register address 0x17D: Reserved
    0x00,        // Register address 0x17E: Reserved
    0x03,        // Register address 0x17F: Page Select
    0x14,        // Register address 0x180: DPLL Hold/Lock Status
    0x00,        // Register address 0x181: External Feedback Control
    0x04,        // Register address 0x182: DPLL Configuration
    0x00,        // Register address 0x183: DPLL Lock Selection
    0x00,        // Register address 0x184: DPLL 1pps Alignment
    0x00,        // Register address 0x185: Reserved
    0x00,        // Register address 0x186: Reserved
    0x00,        // Register address 0x187: Reserved
    0x00,        // Register address 0x188: Reserved
    0x00,        // Register address 0x189: Reserved
    0x00,        // Register address 0x18A: Reserved
    0x00,        // Register address 0x18B: Reserved
    0x00,        // Register address 0x18C: Reserved
    0x00,        // Register address 0x18D: DPLL0 Delta Freq Offset [39:32]
    0x00,        // Register address 0x18E: DPLL0 Delta Freq Offset [31:24]
    0x00,        // Register address 0x18F: DPLL0 Delta Freq Offset [23:16]
    0x00,        // Register address 0x190: DPLL0 Delta Freq Offset [15:8]
    0x00,        // Register address 0x191: DPLL0 Delta Freq Offset [7:0]
    0x00,        // Register address 0x192: DPLL1 Delta Freq Offset [39:32]
    0x00,        // Register address 0x193: DPLL1 Delta Freq Offset [31:24]
    0x00,        // Register address 0x194: DPLL1 Delta Freq Offset [23:16]
    0x00,        // Register address 0x195: DPLL1 Delta Freq Offset [15:8]
    0x00,        // Register address 0x196: DPLL1 Delta Freq Offset [7:0]
    0x00,        // Register address 0x197: DPLL2 Delta Freq Offset [39:32]
    0x00,        // Register address 0x198: DPLL2 Delta Freq Offset [31:24]
    0x00,        // Register address 0x199: DPLL2 Delta Freq Offset [23:16]
    0x00,        // Register address 0x19A: DPLL2 Delta Freq Offset [15:8]
    0x00,        // Register address 0x19B: DPLL2 Delta Freq Offset [7:0]
    0x00,        // Register address 0x19C: DPLL3 Delta Freq Offset [39:32]
    0x00,        // Register address 0x19D: DPLL3 Delta Freq Offset [31:24]
    0x00,        // Register address 0x19E: DPLL3 Delta Freq Offset [23:16]
    0x00,        // Register address 0x19F: DPLL3 Delta Freq Offset [15:8]
    0x00,        // Register address 0x1A0: DPLL3 Delta Freq Offset [7:0]
    0x00,        // Register address 0x1A1: Reserved
    0x00,        // Register address 0x1A2: Reserved
    0x00,        // Register address 0x1A3: Reserved
    0x00,        // Register address 0x1A4: Reserved
    0x00,        // Register address 0x1A5: Reserved
    0x00,        // Register address 0x1A6: Reserved
    0x00,        // Register address 0x1A7: Reserved
    0x00,        // Register address 0x1A8: Reserved
    0x00,        // Register address 0x1A9: Reserved
    0x00,        // Register address 0x1AA: Reserved
    0x00,        // Register address 0x1AB: Reserved
    0x00,        // Register address 0x1AC: Reserved
    0x00,        // Register address 0x1AD: Reserved
    0x00,        // Register address 0x1AE: Reserved
    0x00,        // Register address 0x1AF: Reserved
    0xE4,        // Register address 0x1B0: DPLL->Synth Drive Select
    0x0B,        // Register address 0x1B1: Synthesizer Enable
    0x00,        // Register address 0x1B2: Synthesizer Filter Select
    0x00,        // Register address 0x1B3: Reserved
    0x00,        // Register address 0x1B4: Reserved
    0x00,        // Register address 0x1B5: Synth Prescale Select
    0x02,        // Register address 0x1B6: Synth fail Status
    0x00,        // Register address 0x1B7: Synth Clear Fail Flag
    0x61,        // Register address 0x1B8: Synth0 Base Frequency Bs [15:8]
    0xA8,        // Register address 0x1B9: Synth0 Base Frequency Bs [7:0]
    0x09,        // Register address 0x1BA: Synth0 Freq Multiple Ks [15:8]
    0xC4,        // Register address 0x1BB: Synth0 Freq Multiple Ks [7:0]
    0x00,        // Register address 0x1BC: Synth0 Numerator Ms [15:8]
    0x01,        // Register address 0x1BD: Synth0 Numerator Ms [7:0]
    0x00,        // Register address 0x1BE: Synth0 Denominator Ns [15:8]
    0x01,        // Register address 0x1BF: Synth0 Denominator Ns [7:0]
    0x61,        // Register address 0x1C0: Synth1 Base Frequency [15:8]
    0xA8,        // Register address 0x1C1: Synth1 Base Frequency [7:0]
    0x0C,        // Register address 0x1C2: Synth1 Freq Multiple Ks [15:8]
    0x35,        // Register address 0x1C3: Synth1 Freq Multiple Ks [7:0]
    0x00,        // Register address 0x1C4: Synth1 Numerator Ms [15:8]
    0x01,        // Register address 0x1C5: Synth1 Numerator Ms [7:0]
    0x00,        // Register address 0x1C6: Synth1 Denominator Ns [15:8]
    0x01,        // Register address 0x1C7: Synth1 Denominator Ns [7:0]
    0x9C,        // Register address 0x1C8: Synth2 Base Frequency [15:8]
    0x40,        // Register address 0x1C9: Synth2 Base Frequency [7:0]
    0x07,        // Register address 0x1CA: Synth2 Freq Multiple Ks [15:8]
    0x98,        // Register address 0x1CB: Synth2 Freq Multiple Ks [7:0]
    0x00,        // Register address 0x1CC: Synth2 Numerator Ms [15:8]
    0x01,        // Register address 0x1CD: Synth2 Numerator Ms [7:0]
    0x00,        // Register address 0x1CE: Synth2 Denominator Ns [15:8]
    0x01,        // Register address 0x1CF: Synth2 Denominator Ns [7:0]
    0x61,        // Register address 0x1D0: Synth3 Base Frequency [15:8]
    0xA8,        // Register address 0x1D1: Synth3 Base Frequency [7:0]
    0x09,        // Register address 0x1D2: Synth3 Freq Multiple Ks [15:8]
    0xC4,        // Register address 0x1D3: Synth3 Freq Multiple Ks [7:0]
    0x00,        // Register address 0x1D4: Synth3 Numerator Ms [15:8]
    0x01,        // Register address 0x1D5: Synth3 Numerator Ms [7:0]
    0x00,        // Register address 0x1D6: Synth3 Denominator Ns [15:8]
    0x01,        // Register address 0x1D7: Synth3 Denominator Ns [7:0]
    0x00,        // Register address 0x1D8: Reserved
    0x00,        // Register address 0x1D9: Reserved
    0x00,        // Register address 0x1DA: Reserved
    0x00,        // Register address 0x1DB: Reserved
    0x00,        // Register address 0x1DC: Reserved
    0x00,        // Register address 0x1DD: Reserved
    0x00,        // Register address 0x1DE: Reserved
    0x00,        // Register address 0x1DF: Reserved
    0x00,        // Register address 0x1E0: Reserved
    0x00,        // Register address 0x1E1: Reserved
    0x00,        // Register address 0x1E2: Reserved
    0x00,        // Register address 0x1E3: Reserved
    0x00,        // Register address 0x1E4: Reserved
    0x00,        // Register address 0x1E5: Reserved
    0x00,        // Register address 0x1E6: Reserved
    0x00,        // Register address 0x1E7: Reserved
    0x00,        // Register address 0x1E8: Reserved
    0x00,        // Register address 0x1E9: Reserved
    0x00,        // Register address 0x1EA: Reserved
    0x00,        // Register address 0x1EB: Reserved
    0x00,        // Register address 0x1EC: Reserved
    0x00,        // Register address 0x1ED: Reserved
    0x00,        // Register address 0x1EE: Reserved
    0x00,        // Register address 0x1EF: Reserved
    0x00,        // Register address 0x1F0: Reserved
    0x00,        // Register address 0x1F1: Reserved
    0x00,        // Register address 0x1F2: Reserved
    0x00,        // Register address 0x1F3: Reserved
    0x00,        // Register address 0x1F4: Reserved
    0x00,        // Register address 0x1F5: Reserved
    0x00,        // Register address 0x1F6: Reserved
    0x00,        // Register address 0x1F7: Reserved
    0x00,        // Register address 0x1F8: Reserved
    0x00,        // Register address 0x1F9: Reserved
    0x00,        // Register address 0x1FA: Reserved
    0x00,        // Register address 0x1FB: Reserved
    0x00,        // Register address 0x1FC: Reserved
    0x00,        // Register address 0x1FD: Reserved
    0x00,        // Register address 0x1FE: Reserved
    0x04,        // Register address 0x1FF: Page Select
    0x00,        // Register address 0x200: Synth 0A Post Divide [23:16]
    0x00,        // Register address 0x201: Synth 0A Post Divide [15:8]
    0x08,        // Register address 0x202: Synth 0A Post Divide [7:0]
    0x00,        // Register address 0x203: Synth 0B Post Divide [23:16]
    0x00,        // Register address 0x204: Synth 0B Post Divide [15:8]
    0x08,        // Register address 0x205: Synth 0B Post Divide [7:0]
    0x00,        // Register address 0x206: Synth 0C Post Divide [23:16]
    0x00,        // Register address 0x207: Synth 0C Post Divide [15:8]
    0x28,        // Register address 0x208: Synth 0C Post Divide [7:0]
    0x00,        // Register address 0x209: Synth 0D Post Divide [23:16]
    0x00,        // Register address 0x20A: Synth 0D Post Divide [15:8]
    0x08,        // Register address 0x20B: Synth 0D Post Divide [7:0]
    0x00,        // Register address 0x20C: Synth 1A Post Divide [23:16]
    0x00,        // Register address 0x20D: Synth 1A Post Divide [15:8]
    0x02,        // Register address 0x20E: Synth 1A Post Divide [7:0]
    0x00,        // Register address 0x20F: Synth 1B Post Divide [23:16]
    0x00,        // Register address 0x210: Synth 1B Post Divide [15:8]
    0x02,        // Register address 0x211: Synth 1B Post Divide [7:0]
    0x00,        // Register address 0x212: Synth 1C Post Divide [23:16]
    0x00,        // Register address 0x213: Synth 1C Post Divide [15:8]
    0x32,        // Register address 0x214: Synth 1C Post Divide [7:0]
    0x00,        // Register address 0x215: Synth 1D Post Divide [23:16]
    0x00,        // Register address 0x216: Synth 1D Post Divide [15:8]
    0x32,        // Register address 0x217: Synth 1D Post Divide [7:0]
    0x00,        // Register address 0x218: Synth 2A Post Divide [23:16]
    0x00,        // Register address 0x219: Synth 2A Post Divide [15:8]
    0x00,        // Register address 0x21A: Synth 2A Post Divide [7:0]
    0x00,        // Register address 0x21B: Synth 2B Post Divide [23:16]
    0x00,        // Register address 0x21C: Synth 2B Post Divide [15:8]
    0x00,        // Register address 0x21D: Synth 2B Post Divide [7:0]
    0x00,        // Register address 0x21E: Synth 2C Post Divide [23:16]
    0x00,        // Register address 0x21F: Synth 2C Post Divide [15:8]
    0x00,        // Register address 0x220: Synth 2C Post Divide [7:0]
    0x00,        // Register address 0x221: Synth 2D Post Divide [23:16]
    0x00,        // Register address 0x222: Synth 2D Post Divide [15:8]
    0x00,        // Register address 0x223: Synth 2D Post Divide [7:0]
    0x00,        // Register address 0x224: Synth 3A Post Divide [23:16]
    0x00,        // Register address 0x225: Synth 3A Post Divide [15:8]
    0x08,        // Register address 0x226: Synth 3A Post Divide [7:0]
    0x00,        // Register address 0x227: Synth 3B Post Divide [23:16]
    0x00,        // Register address 0x228: Synth 3B Post Divide [15:8]
    0x08,        // Register address 0x229: Synth 3B Post Divide [7:0]
    0x00,        // Register address 0x22A: Synth 3C Post Divide [23:16]
    0x00,        // Register address 0x22B: Synth 3C Post Divide [15:8]
    0x28,        // Register address 0x22C: Synth 3C Post Divide [7:0]
    0x00,        // Register address 0x22D: Synth 3D Post Divide [23:16]
    0x00,        // Register address 0x22E: Synth 3D Post Divide [15:8]
    0x08,        // Register address 0x22F: Synth 3D Post Divide [7:0]
    0x00,        // Register address 0x230: Reserved
    0x00,        // Register address 0x231: Reserved
    0x00,        // Register address 0x232: Reserved
    0x00,        // Register address 0x233: Reserved
    0x00,        // Register address 0x234: Synth 0C Post Div Phase [15:8]
    0x00,        // Register address 0x235: Synth 0C Post Div Phase [7:0]
    0x00,        // Register address 0x236: Synth 0D Post Div Phase [15:8]
    0x00,        // Register address 0x237: Synth 0D Post Div Phase [7:0]
    0x00,        // Register address 0x238: Reserved
    0x00,        // Register address 0x239: Reserved
    0x00,        // Register address 0x23A: Reserved
    0x00,        // Register address 0x23B: Reserved
    0x00,        // Register address 0x23C: Synth 1C Post Div Phase [15:8]
    0x00,        // Register address 0x23D: Synth 1C Post Div Phase [7:0]
    0x00,        // Register address 0x23E: Synth 1D Post Div Phase [15:8]
    0x00,        // Register address 0x23F: Synth 1D Post Div Phase [7:0]
    0x00,        // Register address 0x240: Reserved
    0x00,        // Register address 0x241: Reserved
    0x00,        // Register address 0x242: Reserved
    0x00,        // Register address 0x243: Reserved
    0x00,        // Register address 0x244: Synth 2C Post Div Phase [15:8]
    0x00,        // Register address 0x245: Synth 2C Post Div Phase [7:0]
    0x00,        // Register address 0x246: Synth 2D Post Div Phase [15:8]
    0x00,        // Register address 0x247: Synth 2D Post Div Phase [7:0]
    0x00,        // Register address 0x248: Reserved
    0x00,        // Register address 0x249: Reserved
    0x00,        // Register address 0x24A: Reserved
    0x00,        // Register address 0x24B: Reserved
    0x00,        // Register address 0x24C: Synth 3C Post Div Phase [15:8]
    0x00,        // Register address 0x24D: Synth 3C Post Div Phase [7:0]
    0x00,        // Register address 0x24E: Synth 3D Post Div Phase [15:8]
    0x00,        // Register address 0x24F: Synth 3D Post Div Phase [7:0]
    0x00,        // Register address 0x250: Synth0 Skew
    0x00,        // Register address 0x251: Synth1 Skew
    0x00,        // Register address 0x252: Synth2 Skew
    0x00,        // Register address 0x253: Synth3 Skew
    0x00,        // Register address 0x254: Synth Stop Clock 1-0
    0x00,        // Register address 0x255: Synth Stop Clock 3-2
    0x00,        // Register address 0x256: Reserved
    0x00,        // Register address 0x257: Reserved
    0x00,        // Register address 0x258: Reserved
    0x00,        // Register address 0x259: Reserved
    0x00,        // Register address 0x25A: Reserved
    0x00,        // Register address 0x25B: Reserved
    0x00,        // Register address 0x25C: Reserved
    0x00,        // Register address 0x25D: Reserved
    0x00,        // Register address 0x25E: Reserved
    0x00,        // Register address 0x25F: Reserved
    0x00,        // Register address 0x260: Reserved
    0xC3,        // Register address 0x261: HP Diff Output Enable
    0x03,        // Register address 0x262: HP CMOS Output Enable
    0xFF,        // Register address 0x263: HP Ouput Reduced Drive
    0xFF,        // Register address 0x264: HP CMOS Drive 3-0
    0xFF,        // Register address 0x265: HP CMOS Drive 7-4
    0x00,        // Register address 0x266: GPIO-0 Select/Status
    0x00,        // Register address 0x267: GPIO-1 Select/Status
    0x60,        // Register address 0x268: GPIO-2 Select/Status
    0x00,        // Register address 0x269: GPIO-3 Select/Status
    0x00,        // Register address 0x26A: GPIO-4 Select/Status
    0x00,        // Register address 0x26B: GPIO-5 Select/Status
    0x00,        // Register address 0x26C: GPIO-6 Select/Status
    0x00,        // Register address 0x26D: Reserved
    0x00,        // Register address 0x26E: Reserved
    0x00,        // Register address 0x26F: Reserved
    0x00,        // Register address 0x270: Reserved
    0x00,        // Register address 0x271: Reserved
    0x00,        // Register address 0x272: Reserved
    0x00,        // Register address 0x273: Reserved
    0x00,        // Register address 0x274: Reserved
    0x00,        // Register address 0x275: Reserved
    0x00,        // Register address 0x276: GPIO Input 7-0
    0xF7,        // Register address 0x277: Reserved
    0x00,        // Register address 0x278: GPIO Output 7-0
    0x00,        // Register address 0x279: Reserved
    0x00,        // Register address 0x27A: GPIO Output Enable 7-0
    0x00,        // Register address 0x27B: Reserved
    0x00,        // Register address 0x27C: GPIO Freeze 7-0
    0x00,        // Register address 0x27D: Reserved
    0x00,        // Register address 0x27E: Reserved
    0x05,        // Register address 0x27F: Page Select
    0x00,        // Register address 0x280: Reserved
    0x00,        // Register address 0x281: Reserved
    0x00,        // Register address 0x282: Reserved
    0x00,        // Register address 0x283: Reserved
    0x00,        // Register address 0x284: Reserved
    0x00,        // Register address 0x285: Reserved
    0x00,        // Register address 0x286: Reserved
    0x00,        // Register address 0x287: Reserved
    0x00,        // Register address 0x288: Reserved
    0x00,        // Register address 0x289: Reserved
    0x00,        // Register address 0x28A: Reserved
    0x00,        // Register address 0x28B: Reserved
    0x00,        // Register address 0x28C: Reserved
    0x00,        // Register address 0x28D: Reserved
    0x00,        // Register address 0x28E: Reserved
    0x00,        // Register address 0x28F: Reserved
    0x00,        // Register address 0x290: Reserved
    0x00,        // Register address 0x291: Reserved
    0x00,        // Register address 0x292: Reserved
    0x00,        // Register address 0x293: Reserved
    0x00,        // Register address 0x294: Reserved
    0x00,        // Register address 0x295: Reserved
    0x00,        // Register address 0x296: Reserved
    0x00,        // Register address 0x297: Reserved
    0x00,        // Register address 0x298: Reserved
    0x00,        // Register address 0x299: Reserved
    0x00,        // Register address 0x29A: Reserved
    0x00,        // Register address 0x29B: Reserved
    0x00,        // Register address 0x29C: Reserved
    0x00,        // Register address 0x29D: Reserved
    0x00,        // Register address 0x29E: Reserved
    0x00,        // Register address 0x29F: Reserved
    0x00,        // Register address 0x2A0: Reserved
    0x00,        // Register address 0x2A1: Reserved
    0x00,        // Register address 0x2A2: Reserved
    0x00,        // Register address 0x2A3: Reserved
    0x00,        // Register address 0x2A4: Reserved
    0x00,        // Register address 0x2A5: Reserved
    0x00,        // Register address 0x2A6: Reserved
    0x00,        // Register address 0x2A7: Reserved
    0x00,        // Register address 0x2A8: Reserved
    0x00,        // Register address 0x2A9: Reserved
    0x00,        // Register address 0x2AA: Reserved
    0x00,        // Register address 0x2AB: Reserved
    0x00,        // Register address 0x2AC: Reserved
    0x00,        // Register address 0x2AD: Reserved
    0x00,        // Register address 0x2AE: Reserved
    0x00,        // Register address 0x2AF: Reserved
    0x00,        // Register address 0x2B0: Reserved
    0x00,        // Register address 0x2B1: Reserved
    0x00,        // Register address 0x2B2: Reserved
    0x00,        // Register address 0x2B3: Reserved
    0x00,        // Register address 0x2B4: Reserved
    0x00,        // Register address 0x2B5: Reserved
    0x00,        // Register address 0x2B6: Reserved
    0x00,        // Register address 0x2B7: Reserved
    0x00,        // Register address 0x2B8: Reserved
    0x00,        // Register address 0x2B9: Reserved
    0x00,        // Register address 0x2BA: Reserved
    0x00,        // Register address 0x2BB: Reserved
    0x00,        // Register address 0x2BC: Reserved
    0x00,        // Register address 0x2BD: Reserved
    0x00,        // Register address 0x2BE: Reserved
    0x00,        // Register address 0x2BF: Reserved
    0x00,        // Register address 0x2C0: Reserved
    0x00,        // Register address 0x2C1: Reserved
    0x00,        // Register address 0x2C2: Reserved
    0x00,        // Register address 0x2C3: Reserved
    0x00,        // Register address 0x2C4: Reserved
    0x00,        // Register address 0x2C5: Reserved
    0x00,        // Register address 0x2C6: Reserved
    0x00,        // Register address 0x2C7: Reserved
    0x00,        // Register address 0x2C8: Reserved
    0x00,        // Register address 0x2C9: Reserved
    0x00,        // Register address 0x2CA: Reserved
    0x00,        // Register address 0x2CB: Reserved
    0x00,        // Register address 0x2CC: Reserved
    0x00,        // Register address 0x2CD: Reserved
    0x00,        // Register address 0x2CE: Reserved
    0x00,        // Register address 0x2CF: Reserved
    0x00,        // Register address 0x2D0: Reserved
    0x00,        // Register address 0x2D1: Reserved
    0x00,        // Register address 0x2D2: Reserved
    0x00,        // Register address 0x2D3: Reserved
    0x00,        // Register address 0x2D4: Reserved
    0x00,        // Register address 0x2D5: Reserved
    0x00,        // Register address 0x2D6: Reserved
    0x00,        // Register address 0x2D7: Reserved
    0x00,        // Register address 0x2D8: Reserved
    0x00,        // Register address 0x2D9: Reserved
    0x00,        // Register address 0x2DA: Reserved
    0x00,        // Register address 0x2DB: Reserved
    0x00,        // Register address 0x2DC: Reserved
    0x00,        // Register address 0x2DD: Reserved
    0x00,        // Register address 0x2DE: Reserved
    0x00,        // Register address 0x2DF: Reserved
    0x00,        // Register address 0x2E0: Reserved
    0x00,        // Register address 0x2E1: Reserved
    0x00,        // Register address 0x2E2: Reserved
    0x00,        // Register address 0x2E3: Reserved
    0x00,        // Register address 0x2E4: Reserved
    0x00,        // Register address 0x2E5: Reserved
    0x00,        // Register address 0x2E6: Reserved
    0x00,        // Register address 0x2E7: Reserved
    0x00,        // Register address 0x2E8: Reserved
    0x00,        // Register address 0x2E9: Reserved
    0x00,        // Register address 0x2EA: Reserved
    0x00,        // Register address 0x2EB: Reserved
    0x00,        // Register address 0x2EC: Reserved
    0x00,        // Register address 0x2ED: Reserved
    0x00,        // Register address 0x2EE: Reserved
    0x00,        // Register address 0x2EF: Reserved
    0x00,        // Register address 0x2F0: Reserved
    0x00,        // Register address 0x2F1: Reserved
    0x00,        // Register address 0x2F2: Reserved
    0x00,        // Register address 0x2F3: Reserved
    0x00,        // Register address 0x2F4: Reserved
    0x00,        // Register address 0x2F5: Reserved
    0x00,        // Register address 0x2F6: Reserved
    0x00,        // Register address 0x2F7: Reserved
    0x00,        // Register address 0x2F8: Reserved
    0x00,        // Register address 0x2F9: Reserved
    0x00,        // Register address 0x2FA: Reserved
    0x00,        // Register address 0x2FB: Reserved
    0x00,        // Register address 0x2FC: Reserved
    0x00,        // Register address 0x2FD: Reserved
    0x00,        // Register address 0x2FE: Reserved
    0x00         // Register address 0x2FF: Reserved
};

/*==============================================================================
 * ZL30362 registers are 8-bit registers.
 */
#define ZL_ADDRESS_BITS_MASK    0x0000007Fu
#define ZL_REG_BITS_MASK        0x000000FFu
#define ZL_READY_CODE           0x0000009Fu

#define READ_ZL_READY_REG_CMD   0x00008000u
#define SELECT_ZL_PAGE0_CMD     0x00007F00u

/*==============================================================================
 * Configure the ZL30362 device through its SPI interface.
 */
void configure_zl30362(void)
{
    uint32_t inc;
    uint32_t zl_register;
    uint32_t zl_ready;
    const uint8_t frame_size = 16u;

    int count  = 0;
    
    /*--------------------------------------------------------------------------
     * Configure SPI interface.
     */
    MSS_SPI_init(&g_mss_spi1);
    
    MSS_SPI_configure_master_mode(&g_mss_spi1,
                                  MSS_SPI_SLAVE_0,
                                  MSS_SPI_MODE0,
                                  MSS_SPI_PCLK_DIV_256,
                                  frame_size);
    
	/* printf("%s %d %s\n", __FILE__, __LINE__, __FUNCTION__); */
    MSS_SPI_set_slave_select(&g_mss_spi1, MSS_SPI_SLAVE_0);
    
    /*--------------------------------------------------------------------------
     * Wait for ZL30362 to become ready.
     * Compare against full ready register value instead of simply polling the
     * ready bit. This avoids false ready bit detection when the ZL device has
     * not fully come out of reset and does not respond to SPI transactions yet.
     * The SPI transfer would typically return all 1's when there is no SPI
     * device responding to the transaction.
     */
    MSS_SPI_transfer_frame(&g_mss_spi1, SELECT_ZL_PAGE0_CMD);
    do {
        zl_register = MSS_SPI_transfer_frame(&g_mss_spi1, READ_ZL_READY_REG_CMD);
        zl_ready = zl_register  & ZL_REG_BITS_MASK;
	/* if (count++ > 1000)  { */
	/* 	printf ("finish timeout\n"); */
	/* 	return; */
	/* } */
    } while(zl_ready != ZL_READY_CODE);
    
    /*--------------------------------------------------------------------------
     * Transfer ZL30362 configuration via SPI.
     */
    for(inc = 0; inc < sizeof(g_zl30362_config); ++inc)
    {
	uint32_t tx_bits;
        
        tx_bits = ((inc & ZL_ADDRESS_BITS_MASK) << 8u) | g_zl30362_config[inc];
        MSS_SPI_transfer_frame(&g_mss_spi1, tx_bits);
    }
}
