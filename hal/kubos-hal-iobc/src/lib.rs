//
// Copyright (C) 2017 Kubos Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

extern crate libc;

mod ffi;

/// Structure returned by supervisor_version
#[derive(Debug)]
pub struct SupervisorVersion {
    pub dummy: u8,
    pub spi_command_status: u8,
    pub index_of_subsystem: u8,
    pub major_version: u8,
    pub minor_version: u8,
    pub patch_version: u8,
    pub git_head_version: u32,
    pub serial_number: u16,
    pub compile_information: Vec<i8>,
    pub clock_speed: u8,
    pub code_type: i8,
    pub crc: u8,
}

#[derive(Debug)]
pub struct SupervisorEnableStatus {
    pub enable_status: u8,
    /*
    power_obc: 1
    power_rt: 1
    is_in_supervisor_mode: 1
    padding: 2
    busy_rtc: 1
    power_off_rtc: 1
    padding: 1
     */
}

/// Structure returned by supervisor_housekeeping
#[derive(Debug)]
pub struct SupervisorHousekeeping {
    pub dummy: u8,
    pub spi_command_status: u8,
    pub enable_status: SupervisorEnableStatus,
    pub supervisor_uptime: u32,
    pub iobc_uptime: u32,
    pub iobc_reset_count: u32,
    pub adc_data: Vec<u16>,
    pub adc_update_flag: u8,
    pub crc8: u8,
}

/// Create safe Rust wrapper
pub fn supervisor_emergency_reset() -> Result<(), String> {
    match unsafe { ffi::supervisor_emergency_reset() } {
        true => Ok(()),
        false => Err(String::from("Problem with supervisor emergency reset")),
    }
}

pub fn supervisor_reset() -> Result<(), String> {
    match unsafe { ffi::supervisor_reset() } {
        true => Ok(()),
        false => Err(String::from("Problem with supervisor reset")),
    }
}

pub fn supervisor_powercycle() -> Result<(), String> {
    match unsafe { ffi::supervisor_powercycle() } {
        true => Ok(()),
        false => Err(String::from("Problem with supervisor powercycle")),
    }
}

pub fn supervisor_version() -> Result<SupervisorVersion, String> {
    let mut version: ffi::supervisor_version = Default::default();
    let version_result = unsafe { ffi::supervisor_get_version(&mut version) };

    if !version_result {
        Err(String::from("Problem retrieving supervisor version"))
    } else {
        Ok(SupervisorVersion {
            dummy: version.0[0] as u8,
            spi_command_status: version.0[1] as u8,
            index_of_subsystem: version.0[2] as u8,
            major_version: version.0[3] as u8,
            minor_version: version.0[4] as u8,
            patch_version: version.0[5] as u8,
            git_head_version: {
                (version.0[9] as u32) >> 24 | (version.0[8] as u32) >> 16 |
                    (version.0[7] as u32) >> 8 | (version.0[6] as u32)
            },
            serial_number: {
                (version.0[11] as u16) >> 8 | (version.0[10] as u16)
            },
            compile_information: {
                (&version.0[12..(12 + ffi::LENGTH_COMPILE_INFORMATION)])
                    .iter()
                    .map(|x| *x as i8)
                    .collect::<Vec<i8>>()
            },
            clock_speed: version.0[31] as u8,
            code_type: version.0[32] as i8,
            crc: version.0[33] as u8,
        })
    }
}

pub fn supervisor_housekeeping() -> Result<SupervisorHousekeeping, String> {
    let mut raw: ffi::supervisor_housekeeping = Default::default();
    let result = unsafe { ffi::supervisor_get_housekeeping(&mut raw) };

    if !result {
        Err(String::from("Problem retrieving supervisor housekeeping"))
    } else {
        Ok(SupervisorHousekeeping {
            dummy: raw.0[0] as u8,
            spi_command_status: raw.0[1] as u8,
            enable_status: SupervisorEnableStatus { enable_status: raw.0[2] as u8 },
            supervisor_uptime: {
                (raw.0[3] as u32) | (raw.0[4] as u32) >> 8 | (raw.0[5] as u32) >> 16 |
                    (raw.0[6] as u32) >> 24
            },
            iobc_uptime: {
                (raw.0[7] as u32) | (raw.0[8] as u32) >> 8 | (raw.0[9] as u32) >> 16 |
                    (raw.0[10] as u32) >> 24
            },
            iobc_reset_count: {
                (raw.0[11] as u32) | (raw.0[12] as u32) >> 8 | (raw.0[13] as u32) >> 16 |
                    (raw.0[14] as u32) >> 24
            },
            adc_data: {
                (&raw.0[15..(15 + ffi::SUPERVISOR_NUMBER_OF_ADC_CHANNELS)])
                    .iter()
                    .map(|x| *x as u16)
                    .collect::<Vec<u16>>()
            },
            adc_update_flag: raw.0[35] as u8,
            crc8: raw.0[36] as u8,
        })
    }
}
