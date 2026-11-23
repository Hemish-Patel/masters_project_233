# Electrical & Control Wiring Working Notes

> Working document synchronized with [ElectricalWiringSchematic.kicad_sch](ElectricalWiringSchematic/ElectricalWiringSchematic.kicad_sch) and `RobotControlArchitecture_HighLevel_Draft.png`.
> Selected components use their exact part numbers; undecided components are prefixed with `Unspecified_`.

---

## 1. Complete Box & Pin Inventory (KiCad Schematic)

### Part 1: Power Distribution Subsystem (Tethered, Galvanically Isolated)

| Box / Symbol Name in KiCad | Schematic Ref & Label (`Value`) | Qty | Status | Power & Ground Wiring | Notes |
| :--- | :--- | :---: | :---: | :--- | :--- |
| **`Unspecified_Tethered_24V_PSU`** | `PS1`: `Unspecified_Tethered_24V_PSU` | 1 | **Wired (Model TBD)** | `+24V_OUT` $\rightarrow$ `+24V` rail (`U1` Pin `1` + `U2–U5` `+Vdc_24V`)<br>`GND` $\rightarrow$ **`24V_GND`** (`U1` Pin `2` + `U2–U5` `GND`) | Target spec: **`24 V / 10 A` (`240 W`)** regulated DC supply *(e.g. Mean Well `GST280A24-C6P` or `LRS-350-24`)* |
| **`MeanWell_SKMW40F_05`** | `U1`: `Mean Well SKMW40F-05 (24V->5V 40W)` | 1 *(RS `624-558`)* | **Selected & Wired** | **Pin `1` (`+Vin`)** $\rightarrow$ `+24V` rail *(`9–36 V DC` in; add `4 A` fuse + Schottky diode)*<br>**Pin `2` (`-Vin`)** $\rightarrow$ **`24V_GND`** (isolated input return)<br>**Pin `3` (`R.C`)** $\rightarrow$ `NC` *(open = ON; short/`<1.2 V` to `-Vin` = OFF)*<br>**Pin `6` (`+Vout`)** $\rightarrow$ **`+5V`** rail (`5.0 V ± 1.0%`, `8 A`, `40 W`)<br>**Pin `4` (`-Vout`)** $\rightarrow$ **`5V_GND`** (isolated output return)<br>**Pin `5` (`Trim`)** $\rightarrow$ `NC` *(open = fixed `5.0 V`)* | `25.4 × 25.4 × 10 mm` `1"×1"` DIP (`21 g`, `88%` typ eff., **`2 kVDC` galvanic isolation** between `24V_GND` and `5V_GND`); *see Section 5 for mandatory fuse/diode & heatsink warnings* |

---

### Part 2: Standalone Manually Controlled Robot (`NUCLEO-L432KC` Subsystem)

| Box / Symbol Name in KiCad | Schematic Ref & Label (`Value`) | Qty | Status | Power & Ground Wiring | Signal / Control Wiring Status |
| :--- | :--- | :---: | :---: | :--- | :--- |
| **`NEMA17_17HE19_2004S`** | `M1–M3`: `Wheel_Motor_1..3 (17HE19-2004S)`<br>`M4`: `Gantry_Motor (17HE19-2004S)` | 4 | **Selected & Wired** | Driven via `DM320T` coil outputs (`2.0 A/phase`, `1.30 Ω`) | **Wired to `U2–U5`:**<br>`A+_BLK` $\rightarrow$ `A+`, `A-_BLU` $\rightarrow$ `A-`, `B+_GRN` $\rightarrow$ `B+`, `B-_RED` $\rightarrow$ `B-` |
| **`DM320T_Driver`** | `U2–U4`: `Wheel_Driver_1..3 (DM320T)`<br>`U5`: `Gantry_Driver (DM320T)` | 4 | **Selected & Wired** | **Motor Stage (`24V` domain):** `+Vdc_24V` $\rightarrow$ `+24V`, `GND` $\rightarrow$ `24V_GND`<br>**Opto Stage (`5V` isolated domain):** `OPTO_5V` $\rightarrow$ `+5V` | **To Motors (`M1–M4`):** `A+`, `A-`, `B+`, `B-` (Wired)<br>**From `TBD62083APG` (`U19`):** `PUL`, `DIR` (Wired individually via `O1..O8`)<br>**`ENA`:** Left **`NC`** (enabled by default per `DM320T` datasheet) |
| **`TBD62083APG`** | `U19`: `TBD62083APG (8-Ch DMOS Driver)` | 1 *(8-ch, PDIP-18)* | **Selected & Wired** | `COM_5V` (Pin 10) $\rightarrow$ `+5V` rail *(internal clamp diodes)*<br>`GND` (Pin 9) $\rightarrow$ **`5V_GND`** *(no `3.3V` power pin needed)* | **Inputs (`3.3V` from `U11`):** `I1..I8` (Pins `1..8`, $V_{IN(\text{ON})} \ge 2.5\text{ V}$, $V_{IN(\text{OFF})} \le 0.6\text{ V}$)<br>**Outputs (Open-drain sink to `U2–U5`):** `O1..O8` (Pins `18..11`, `500 mA`/ch, $R_{\text{ON}} \approx 2\ \Omega$, $t_{\text{ON}}/t_{\text{OFF}} = 0.4/0.8\ \mu\text{s}$)<br>*Active logic:* MCU `HIGH` (`3.3 V`) $\rightarrow$ `O1..O8` sinks `PUL`/`DIR` `LOW` $\rightarrow$ `DM320T` opto LED **ON** |
| **`AS5600_Encoder`** | `U6–U8`: `Wheel_Enc_1..3 (AS5600)` | 3 | **Selected & Wired** | `VCC` $\rightarrow$ `+3.3V` rail<br>`GND` $\rightarrow$ **`5V_GND`** | **Analog Mode (Wired):**<br>`OUT` $\rightarrow$ `U11` `A0/PA0`, `A1/PA1`, `A2/PA3`<br>`DIR` $\rightarrow$ tied directly to `5V_GND` (CW = increasing angle)<br>`SCL`, `SDA` $\rightarrow$ **`NC`** (fixed I2C address `0x36` conflict) |
| **`ISM330DHCX_IMU_Module`** | `U9`: `ISM330DHCX (6-Axis IMU)` | 1 *(Breakout)* | **Selected & Wired** | `VCC_3V3` (Pin 1) $\rightarrow$ `+3.3V` rail *(`1.71–3.6 V` max)*<br>`GND` (Pin 2) $\rightarrow$ **`5V_GND`** | **I2C Mode 1 (Wired):**<br>`SDA` (Pin 3) $\leftrightarrow$ `U11` `D4/PB7_SDA` (`CN3-7`)<br>`SCL` (Pin 4) $\leftarrow$ `U11` `D5/PB6_SCL` (`CN3-8`)<br>`INT1`, `INT2`, `CS`, `SDO_SA0` $\rightarrow$ **`NC`** *(breakout pulls `CS` HIGH for I2C and includes I2C pull-ups & decoupling caps)* |
| **`HC05_Bluetooth_Module`** | `U10`: `HC-05 (Bluetooth Serial)` | 1 *(6-pin ZS-040 Breakout)* | **Selected & Wired** | `VCC_5V` (Pin 1) $\rightarrow$ `+5V` rail *(`3.6–6 V` onboard 3.3V LDO)*<br>`GND` (Pin 2) $\rightarrow$ **`5V_GND`** | **UART (`3.3V` logic, Wired directly to `U11` `USART1`):**<br>`TXD` (Pin 3) $\rightarrow$ `U11` `D0/PA10_BT_RX` (`CN3-2`)<br>`RXD` (Pin 4) $\leftarrow$ `U11` `D1/PA9_BT_TX` (`CN3-1`)<br>`STATE` (Pin 5), `EN_KEY` (Pin 6) $\rightarrow$ **`NC`** |
| **`NUCLEO_L432KC`** | `U11`: `NUCLEO-L432KC (Motor MCU)` | 1 *(MB1180, STM32L432KCU6)* | **Selected & Wired (Low-Level RC Core)** | `+5V` (`CN4-4`) $\rightarrow$ `+5V` rail *(Note: `VIN` `CN4-1` is `7–12 V` only, NC)*<br>`+3V3` (`CN4-14`) $\rightarrow$ `+3.3V` rail<br>`GND` (`CN3-4`/`CN4-2`) $\rightarrow$ **`5V_GND`** | **Wired:** 8× stepper `PUL`/`DIR`, 3× `AS5600` ADC, 2× `ISM330DHCX` I2C1, 2× `HC-05` USART1<br>**Pending:** Gantry limit switches (`A6/PA7_LIM_HI`, `D13/PB3_LIM_LO`) & Main Computer link (`USB_VCP` `CN1` — deferred) |

---

### Part 3: Robot Body (`Main Computer` Subsystem — Deferred for Later Discussion)

| Box / Symbol Name in KiCad | Schematic Ref & Label (`Value`) | Qty | Status | Power Pins | Signal / Control Pins (Pending Component Selection) |
| :--- | :--- | :---: | :---: | :--- | :--- |
| **`Unspecified_Main_Computer`** | `U12`: `Unspecified_Main_Computer` | 1 | **Placeholder** | `VCC_5V`, `GND` (`5V_GND`) | **Left:** `MCU_RX`, `MCU_TX`<br>**Right:** `TOF_SDA/SCL`, `US1..2_TRIG/ECHO`, `WIFI_DATA+/-`, `SENSOR_MCU_TX/RX`, `SCAN_TRIG` |
| **`Unspecified_ToF_Sensor`** | `U13`: `Unspecified_HighRes_ToF_SLAM`<br>`U14`: `Unspecified_SinglePt_ToF_SLAM` | 2 | **Placeholder** | `VCC`, `GND` | `SDA`, `SCL` (I2C) |
| **`Unspecified_Ultrasonic_Sensor`** | `U15–U16`: `Unspecified_Ultrasonic_1..2` | 2 | **Placeholder** | `VCC_5V`, `GND` | `TRIG`, `ECHO` |
| **`Unspecified_5GHz_WiFi_Uplink`** | `U17`: `Unspecified_5GHz_WiFi_Uplink` | 1 | **Placeholder** | `VCC_5V`, `GND` | `DATA+`, `DATA-`, `RF_ANT` |
| **`Unspecified_Sensor_Suite_MCU`** | `U18`: `Unspecified_Sensor_Suite_MCU` | 1 | **Obj 4 Black Box** | `VCC_5V`, `GND` | `RX`, `TX`, `SCAN_TRIG` *(1–1.5 m cable run up Z-axis gantry)* |

---

## 2. `NUCLEO-L432KC` (`U11`) & `TBD62083APG` (`U19`) Exact Pin Mapping Table

All 17 out-of-the-box usable header pins of the `NUCLEO-L432KC` (`MB1180`, per ST `UM1956`) are assigned with zero solder-bridge modifications required:

### A. Stepper Control Path (`U11` `3.3V` $\rightarrow$ `U19` `TBD62083APG` DMOS Sink $\rightarrow$ `U2–U5` `DM320T` `5V` Optos)

| Function | `NUCLEO-L432KC` Pin (`U11`) | Header | `TBD62083APG` Input (`3.3V`) | `TBD62083APG` Output (Sink) | `DM320T` Destination |
| :--- | :--- | :---: | :--- | :--- | :--- |
| **Wheel 1 Step** | `D2 / PA12` | `CN3-5` | `I1` (Pin 1) | `O1` (Pin 18) | `U2` (`Wheel_Driver_1`) `PUL` (Pin 8) |
| **Wheel 1 Dir** | `D3 / PB0` | `CN3-6` | `I2` (Pin 2) | `O2` (Pin 17) | `U2` (`Wheel_Driver_1`) `DIR` (Pin 9) |
| **Wheel 2 Step** | `D6 / PB1` | `CN3-9` | `I3` (Pin 3) | `O3` (Pin 16) | `U3` (`Wheel_Driver_2`) `PUL` (Pin 8) |
| **Wheel 2 Dir** | `D9 / PA8` | `CN3-12` | `I4` (Pin 4) | `O4` (Pin 15) | `U3` (`Wheel_Driver_2`) `DIR` (Pin 9) |
| **Wheel 3 Step** | `D10 / PA11` | `CN3-13` | `I5` (Pin 5) | `O5` (Pin 14) | `U4` (`Wheel_Driver_3`) `PUL` (Pin 8) |
| **Wheel 3 Dir** | `D11 / PB5` | `CN3-14` | `I6` (Pin 6) | `O6` (Pin 13) | `U4` (`Wheel_Driver_3`) `DIR` (Pin 9) |
| **Gantry Step** | `D12 / PB4` | `CN3-15` | `I7` (Pin 7) | `O7` (Pin 12) | `U5` (`Gantry_Driver`) `PUL` (Pin 8) |
| **Gantry Dir** | `A3 / PA4` | `CN4-9` | `I8` (Pin 8) | `O8` (Pin 11) | `U5` (`Gantry_Driver`) `DIR` (Pin 9) |
| **Power / Return** | — | — | `COM_5V` (Pin 10) $\rightarrow$ `+5V` | `GND` (Pin 9) $\rightarrow$ `5V_GND` | *`DM320T` `ENA` left `NC` (enabled by default)* |

### B. Sensors, Bluetooth Remote & Limit Switches (`U11` `3.3V` Direct)

| Function | `NUCLEO-L432KC` Pin (`U11`) | Header | Peripheral Connection | Notes |
| :--- | :--- | :---: | :--- | :--- |
| **Wheel 1 Encoder** | `A0 / PA0` (`ADC1_IN5`) | `CN4-12` | `U6` (`AS5600`) `OUT` (Pin 3) | 0–3.3 V analog angle (`DIR` tied to `5V_GND`) |
| **Wheel 2 Encoder** | `A1 / PA1` (`ADC1_IN6`) | `CN4-11` | `U7` (`AS5600`) `OUT` (Pin 3) | 0–3.3 V analog angle (`DIR` tied to `5V_GND`) |
| **Wheel 3 Encoder** | `A2 / PA3` (`ADC1_IN8`) | `CN4-10` | `U8` (`AS5600`) `OUT` (Pin 3) | 0–3.3 V analog angle (`PA3` is 3.3V-only, NOT 5V-tolerant) |
| **IMU I2C Data** | `D4 / PB7` (`I2C1_SDA`) | `CN3-7` | `U9` (`ISM330DHCX`) `SDA` (Pin 3) | Bridged to `A4/PA5` via `SB18` (ON by default) — keep `PA5` input-floating in firmware (or open `SB18`) |
| **IMU I2C Clock** | `D5 / PB6` (`I2C1_SCL`) | `CN3-8` | `U9` (`ISM330DHCX`) `SCL` (Pin 4) | Bridged to `A5/PA6` via `SB16` (ON by default) — keep `PA6` input-floating in firmware (or open `SB16`) |
| **Bluetooth TX** | `D1 / PA9` (`USART1_TX`) | `CN3-1` | `U10` (`HC-05`) `RXD` (Pin 4) | 3.3V logic — direct connection, no divider needed |
| **Bluetooth RX** | `D0 / PA10` (`USART1_RX`) | `CN3-2` | `U10` (`HC-05`) `TXD` (Pin 3) | 3.3V logic — direct connection |
| **Gantry Top Limit** | `A6 / PA7` (GPIO In) | `CN4-6` | *Pending switch placement* | Use internal pull-up |
| **Gantry Bot Limit** | `D13 / PB3` (GPIO In) | `CN4-15` | *Pending switch placement* | Shared with onboard LED `LD3` (`SB15`) |
| **Main Computer** | `CN1` (`USB_VCP` / `PA2`, `PA15`) | `USB` | *Deferred (to `U12`)* | Built-in ST-LINK Virtual COM Port (`USART2`) |

---

## 3. Power Budget Summary (`24V` Input & `5V` Isolated Rail)

- **`5V` Rail (`U1`: Mean Well `SKMW40F-05`, `40 W` / `8 A` rated):**
  - Low-level RC core alone: `~0.22–0.36 A` (`~1.8 W`)
  - Full system (incl. Pi 5 + 5 GHz Wi-Fi + SLAM ToF/Ultrasonic + Obj 4 gantry sensor suite): `~2.7–3.2 A` typ (`~14–16 W`, **~2.5× headroom**), `~6.0–6.5 A` worst-case simultaneous peak (`~30–32.5 W`, **~20–25% headroom** within `40 W / 8 A` rating).
- **`24V` Tethered Input (`PS1`: Target `24 V / 10 A`, `240 W`):**
  - 4× `17HE19-2004S` + `DM320T`: `~1.8–3.3 A` typ, `~5.35 A` peak (`~128 W`)
  - `SKMW40F-05` input (`88%` eff.): `~0.7 A` typ, `~1.5 A` peak (`~36 W`; `1.85 A` max at full `40 W` output per datasheet Page 2)
  - **Total `24 V` supply:** `~2.5–4.0 A` typ (`60–97 W`), `~6.8–7.7 A` peak (`164–184 W`) $\rightarrow$ size `PS1` for **`24 V / 10 A` (`240 W`)** with `14–16 AWG` tether cable and `10 A` main inline fuse.

---

## 4. Datasheet Folder Audit (`DataSheets/`)

All **9 PDFs** in `DataSheets/` are verified official manufacturer documents with zero unused/extra files and zero missing datasheets for all selected components:

| File in `DataSheets/` | Component(s) | Status | Notes |
| :--- | :--- | :---: | :--- |
| `17HE19-2004S_Full_Datasheet.pdf` | `M1–M4` Steppers | **Verified** | Exact StepperOnline drawing (`2.0 A/phase`, `1.30 Ω`, `0.55 N·m`) |
| `DM320T.pdf` | `U2–U5` Drivers | **Verified** | StepperOnline manual (`18–30 V`, `OPTO_5V`, `ENA` NC = enabled) |
| `TBD62083APG.pdf` | `U19` DMOS Sink Driver | **Verified** | Toshiba `PDIP-18`, 8-ch, `3.3 V` input ($V_{IN(\text{ON})} \ge 2.5\text{ V}$), `500 mA`/ch |
| `Mean Well SKMW40F-05.pdf` | `U1` `24V → 5V` DC-DC | **Verified** | Mean Well `SKMW40,DKMW40-SPEC` (`9–36 V` in, `5.0 V ± 1.0% / 8 A`, `88%` eff., `2 kVDC` isolation; exact Page 5 pinout: `1=+Vin`, `2=-Vin`, `3=R.C.`, `4=-Vout`, `5=Trim`, `6=+Vout`) |
| `AS5600_Datasheet.pdf` | `U6–U8` Wheel Encoders | **Verified** | ams Datasheet `[v1-06]` (43 pp) — `3.0–3.6 V` (`6.5 mA` typ), 12-bit ratiometric `OUT` (`0–360°` default), `DIR` = `GND` (CW+), fixed I2C `0x36` |
| `DS_ism330dhcx.pdf` | `U9` 6-Axis IMU | **Verified** | ST `DS13012` (`1.71–3.6 V` I2C Mode 1; schematic assumes breakout PCB) |
| `HC05_DS.pdf` | `U10` Bluetooth Serial | **Verified** | Bare module spec (`3.3 V` UART; schematic assumes 6-pin `5 V` `ZS-040` breakout) |
| `DB_nucleo-l432kc.pdf` | `U11` Motor MCU | **Verified** | ST `DB2638` Data Brief (`MB1180` / `STM32L432KCU6` summary) |
| `UM1956_Nucleo-L432KC_UserManual.pdf` | `U11` Motor MCU | **Verified** | ST `UM1956 Rev 6` (43 pp) — Table 16 / Fig. 8 confirm 100% of `U11` `CN3`/`CN4` pins, power specs (`+5V` `4.75–5.25 V`), and solder bridges (`SB9`, `SB16`, `SB18`) |
| *(Pending Selection)* `PS1`, `U12–U18` | `24V` PSU & Main Computer | **Pending** | Deferred for later; add datasheets once specific parts are chosen |

---

## 5. Key Architectural & Safety Notes

1. **Galvanic Isolation (`24V_GND` vs `5V_GND`):**
   - Because `U1` (`Mean Well SKMW40F-05`) provides **`2 kVDC` I/P–O/P isolation** and `U2–U5` (`DM320T`) have **optically isolated** control inputs, `24V_GND` (Pin `2` `-Vin`) and `5V_GND` (Pin `4` `-Vout`) are intentionally kept **separate** on the schematic to keep high-current stepper switching noise out of the `3.3 V` ADC (`AS5600` encoders) and MCU logic.
2. **Power & Solder-Bridge Notes on `NUCLEO-L432KC` (`U11`, per `UM1956 Rev 6`):**
   - **Standalone `+5V` power without USB (`SB9`):** Per `UM1956` Section 7.4.2 (Page 18) & Table 8 (Page 21), powering `U11` via `+5V` (`CN4-4`, `4.75–5.25 V`) **without USB connected** leaves the ST-LINK unpowered, which can hold `NRST` low unless **`SB9` is opened** (consider fitting a removable 2-pin jumper on `SB9` or using Software Reset in STM32CubeIDE when flashing).
   - **USB + External `+5V` Power Sequence:** When both `+5V` (`CN4-4`) and USB (`CN1`, e.g. PC flashing or future Pi connection) are used together, `UM1956` Section 7.4.2 requires **`SB1` OFF** (default) and powering **`+5V` ON first** (verify red `LD2` lights) **before** plugging in USB `CN1` (or disconnect `+5V` while flashing over USB alone).
   - **`I2C1` on `D4/D5` (`SB16`/`SB18`):** Factory-default `SB16` and `SB18` are **ON**, physically shorting `D5/PB6` $\leftrightarrow$ `A5/PA6` and `D4/PB7` $\leftrightarrow$ `A4/PA5`. Our `ISM330DHCX` wiring to `D4`/`D5` works out of the box provided firmware keeps `PA5` and `PA6` configured as **input floating** (or desolder `SB16`/`SB18`).
3. **`AS5600` (`U6–U8`) `3.3 V` Breakout Check:**
   - Per `AS5600_Datasheet.pdf` Page 9, **`VDD5V` and `VDD3V3` must be tied together for `3.3 V` operation**. Verify the purchased `AS5600` breakout PCBs are configured for `3.3 V` mode (e.g. `0 Ω` bridge `R1` installed on standard white modules; boards shipped for `5 V`-only have `R1` removed and will not power properly from `3.3 V`).
4. **`SKMW40F-05` Mandatory Input Protection & Thermal / Heatsink Warnings:**
   - **Definite need for an input fuse:** `SKMW40F-05` has **no built-in fuse**. Fit a **`4 A` slow-blow (time-lag) fuse** inline on the `+24V` feed to `Pin 1 (+Vin)` for tight protection on our `24 V` rail (`1.85 A` max full-load draw; datasheet Page 3 specifies up to `8 A` delay-time fuse across the `9–36 V` range).
   - **Definite need for a reverse-polarity protection diode:** `SKMW40F-05` has **no built-in reverse-polarity protection** — connecting `+24V` and `24V_GND` backwards will destroy the module immediately. Fit a **series Schottky diode** (e.g. `SB560` / `SR560`, `5 A / 60 V`, `~0.5 V` forward drop — negligible on `24 V` with `9–36 V` input range) in series after the fuse on `+Vin`. *(Optionally add a `10 µF / 50 V` low-ESR capacitor across `Pin 1`/`Pin 2` near the module to damp tether inductance spikes; surge max is `50 V DC` for `100 ms`.)*
   - **Possible need for a heatsink / airflow:** Per the Page 6 derating curve (`Mean Well SKMW40F-05.pdf`), under natural convection (even on a `95.4 × 85 mm` 2 oz Cu PCB), full `100%` load (`40 W`) is rated only up to **`30 °C` ambient** (derating to `50%` / `20 W` at `60 °C`), whereas with a **baseplate/heatsink** it holds `100%` (`40 W`) up to **`60 °C`**. Keep the module in open air, use thick `16–18 AWG` short wires from `Pin 6 (+Vout)` / `Pin 4 (-Vout)` to keep `5.0 V ± 1.0%` clean at the Raspberry Pi 5, and attach a stick-on **`25 × 25 mm` aluminium heatsink** (or fan airflow) once running `> 20–25 W` continuously or inside an enclosed bay above `30 °C`.
