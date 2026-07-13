# M5Faces Gamepad3 I2C Control Protocol

| Document information | Details |
| :--- | :--- |
| Product | M5Faces Gamepad3 |
| SKU | `A004-V3` |
| Applicable firmware | `v03` |
| Interface | I2C |
| Language | English |

---

## 1. Hardware Interface

| Item | Description |
| :--- | :--- |
| Default I2C slave address (7-bit) | `0x08` |
| Configurable address range | `0x08`-`0x77` |
| Bus speed | Standard Mode `100 kbps` / Fast Mode `400 kbps` |
| IRQ active level | Active low |
| Key debounce time | `20 ms` |

### 1.1 IRQ Behavior

When any debounced key state changes (press or release), the module pulls `IRQ` low. After the host directly reads the 1-byte key state, the module clears the pending key event and returns `IRQ` high.

```text
Key state changes                 Host reads key state
        |                                  |
--------+ IRQ low (0) ---------------------+ IRQ high (1)
```

> Reading a configuration register does not clear a pending key event. If `IRQ` is low, the host must still perform a key-state read.

---

## 2. Communication Overview

The module supports three types of I2C operations:

| Operation | Host transaction | Purpose |
| :--- | :--- | :--- |
| Key-state read | Directly read 1 byte | Get the current state of all keys and clear IRQ |
| Register read | Write a 1-byte register address, then read | Get the device ID, UID, firmware version, or current I2C address |
| Command write | Write `[register address] [data]`, 2 bytes total | Trigger a reset or change the I2C address |

All multi-byte lists are transmitted from left to right as shown in the tables.

---

## 3. Key-State Read

When `IRQ` is low, the host directly reads exactly 1 byte from the module without first writing a register address.

### 3.1 Data Format

The key bits are active low:

- `0`: key pressed
- `1`: key released

| Bit | Mask | Key |
| :---: | :---: | :--- |
| Bit 0 | `0x01` | UP |
| Bit 1 | `0x02` | DOWN |
| Bit 2 | `0x04` | LEFT |
| Bit 3 | `0x08` | RIGHT |
| Bit 4 | `0x10` | A |
| Bit 5 | `0x20` | B |
| Bit 6 | `0x40` | SELECT |
| Bit 7 | `0x80` | START |

Multiple keys can be pressed simultaneously. After power-on, the key-state value is `0xFF` when no keys are pressed.

### 3.2 Examples

| Returned value | State |
| :---: | :--- |
| `0xFF` | No keys pressed |
| `0xFE` | UP pressed |
| `0xEF` | A pressed |
| `0xEE` | UP and A pressed simultaneously |

Test whether a key is pressed:

```c
bool a_pressed = (key_state & 0x10U) == 0U;
```

---

## 4. Register Address Map

| Register address | Readable | Writable | Default / returned value | Description |
| :--- | :---: | :---: | :--- | :--- |
| `0xD0` | Yes | No | `0x03` | Device type ID (Gamepad) |
| `0xE0`-`0xEB` | Yes | No | Device-specific | 96-bit MCU UID, 12 bytes total |
| `0xFD` | No | Yes | - | System reset / IAP entry command |
| `0xFE` | Yes | No | `0x03` | Firmware version |
| `0xFF` | Yes | Yes | `0x08` | I2C address register |

> Addresses `0xD1`-`0xDF` and `0xEC`-`0xFC` are undefined and must not be accessed.

---

## 5. Register Reads

### 5.1 Read Procedure

1. The host writes a 1-byte register address to the module.
2. The host issues a repeated START or a new I2C read transaction.
3. The module returns data from the selected register block.

The host must not read more than the maximum length specified below.

### 5.2 Readable Register Blocks

#### Device Type ID

| Start address | Readable bytes | Returned data |
| :--- | :---: | :--- |
| `0xD0` | 1 | `[0x03]` |

#### MCU UID

| Start address | Maximum readable bytes | Returned data |
| :--- | :---: | :--- |
| `0xE0`-`0xEB` | `0xEC - start address` | Remaining UID data starting at the corresponding offset |

The UID is read into a cache at power-on. To read the complete UID, start at `0xE0` and read 12 bytes. The byte order follows ascending STM32 UID storage addresses.

#### Firmware Version and I2C Address

| Start address | Readable bytes | Returned data |
| :--- | :---: | :--- |
| `0xFE` | 2 | `[firmware_version, i2c_address]` |
| `0xFF` | 1 | `[i2c_address]` |

Firmware `v03` returns `0x03` as `firmware_version`. `i2c_address` is the current 7-bit slave address and does not include the read/write direction bit.

---

## 6. Command Writes

The command write format is exactly 2 bytes:

```text
[register address] [data]
```

Writes of other lengths are not executed as valid commands.

### 6.1 System Reset / IAP Entry (`0xFD`)

| Written data | Behavior |
| :---: | :--- |
| `0x00` | No operation |
| `0x01`-`0xFF` | Immediately trigger an MCU system reset and enter the boot flow |

Example:

```text
[0xFD] [0x01]
```

> This document defines only the reset entry point in the application firmware. It does not define the subsequent IAP firmware transfer protocol.

### 6.2 Change I2C Address (`0xFF`)

| Item | Description |
| :--- | :--- |
| Valid range | `0x08`-`0x77` |
| Effective time | Immediately after the write |
| Retained after power-off | Yes, stored in internal Flash |

Example: change the module address to `0x20`:

```text
[0xFF] [0x20]
```

After the write completes, the host must use the new address for further communication. Wait until the current write transaction has fully completed before accessing the new address. Values outside the valid range are ignored.

---

## 7. Host Access Examples

All addresses in the following pseudocode are 7-bit I2C addresses.

### 7.1 Read Key State

```c
uint8_t key_state;
i2c_read(device_address, &key_state, 1);
```

### 7.2 Read Firmware Version and Current Address

```c
uint8_t reg = 0xFE;
uint8_t info[2];
i2c_write(device_address, &reg, 1);
i2c_read(device_address, info, 2);
```

### 7.3 Read the Complete UID

```c
uint8_t reg = 0xE0;
uint8_t uid[12];
i2c_write(device_address, &reg, 1);
i2c_read(device_address, uid, 12);
```

---

## 8. Usage Notes

- The host should read the 1-byte key state after detecting that `IRQ` is low.
- Both key presses and releases generate state-change events; the host must handle both.
- If multiple keys change together, the returned byte represents the complete current state at read time, not an individual key event.
- Register reads and key-state reads use different transaction flows and must not be mixed.
- After changing the I2C address, retain or rediscover the new address so the device remains reachable after subsequent startups.
