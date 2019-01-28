#include "SckUrban.h"
#include "SckBase.h"

// Hardware Serial UART PM
Uart SerialPM (&sercom5, pinPM_SERIAL_RX, pinPM_SERIAL_TX, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM5_Handler() {
	SerialPM.IrqHandler();
}

bool SckUrban::present()
{
	SerialUSB.println("detecting urban");

	if ( 	!I2Cdetect(&Wire, sck_bh1721fvc.address) ||
		!I2Cdetect(&Wire, sck_sht31.address) ||
		!I2Cdetect(&Wire, sck_mpl3115A2.address) ||
		!I2Cdetect(&Wire, sck_max30105.address)) {
		return false;
	}
	return true;
}

bool SckUrban::setup()
{
	// To protect MICS turn off heaters
	sck_mics4514.startPWM(); 	// Workaround Noise not working without mics init
	sck_mics4514.stop(rtc->getEpoch());

	if (!sck_bh1721fvc.start()) return false;
	if (!sck_sht31.start()) return false;
	if (!sck_mics4514.start(rtc->getEpoch())) return false;
	if (!sck_noise.start()) return false;
	if (!sck_mpl3115A2.start()) return false;
	if (!sck_max30105.start()) return false;
		
	return true;
};
bool SckUrban::start(SensorType wichSensor)
{
	switch(wichSensor) {
		case SENSOR_LIGHT: 				if (sck_bh1721fvc.start()) return true; break;
		case SENSOR_TEMPERATURE:
		case SENSOR_HUMIDITY: 				if (sck_sht31.start()) return true; break;
		case SENSOR_CO_RESISTANCE:
		case SENSOR_CO_HEAT_VOLT:
		case SENSOR_CO_HEAT_TIME:
		case SENSOR_NO2_RESISTANCE:
		case SENSOR_NO2_HEAT_VOLT:
		case SENSOR_NO2_HEAT_TIME:
		case SENSOR_NO2_LOAD_RESISTANCE:		if (sck_mics4514.start(rtc->getEpoch())) return true; break;
		case SENSOR_NOISE_DBA:
		case SENSOR_NOISE_DBC:
		case SENSOR_NOISE_DBZ:
		case SENSOR_NOISE_FFT: 				if (sck_noise.start()) return true; break;
		case SENSOR_ALTITUDE:
		case SENSOR_PRESSURE:
		case SENSOR_PRESSURE_TEMP: 			if (sck_mpl3115A2.start()) return true; break;
		case SENSOR_PARTICLE_RED:
		case SENSOR_PARTICLE_GREEN:
		case SENSOR_PARTICLE_IR:
		case SENSOR_PARTICLE_TEMPERATURE: 		if (sck_max30105.start()) return true; break;
		case SENSOR_PM_1:
		case SENSOR_PM_25:
		case SENSOR_PM_10:
		case SENSOR_PN_03:
		case SENSOR_PN_05:
		case SENSOR_PN_1:
		case SENSOR_PN_25:
		case SENSOR_PN_5:
		case SENSOR_PN_10: 				if (sck_pm.start()) return true; break;
		default: break;
	}

	return false;
}
bool SckUrban::stop(SensorType wichSensor)
{
	switch(wichSensor) {
		case SENSOR_LIGHT: 				if (sck_bh1721fvc.stop()) return true; break;
		case SENSOR_TEMPERATURE:
		case SENSOR_HUMIDITY: 				if (sck_sht31.stop()) return true; break;
		case SENSOR_CO_RESISTANCE:
		case SENSOR_CO_HEAT_VOLT:
		case SENSOR_CO_HEAT_TIME:
		case SENSOR_NO2_RESISTANCE:
		case SENSOR_NO2_HEAT_VOLT:
		case SENSOR_NO2_HEAT_TIME:
		case SENSOR_NO2_LOAD_RESISTANCE:		if (sck_mics4514.stop(rtc->getEpoch())) return true; break;
		case SENSOR_NOISE_DBA:
		case SENSOR_NOISE_DBC:
		case SENSOR_NOISE_DBZ:
		case SENSOR_NOISE_FFT: 				if (sck_noise.stop()) return true;
		case SENSOR_ALTITUDE:
		case SENSOR_PRESSURE:
		case SENSOR_PRESSURE_TEMP: 			if (sck_mpl3115A2.stop()) return true; break;
		case SENSOR_PARTICLE_RED:
		case SENSOR_PARTICLE_GREEN:
		case SENSOR_PARTICLE_IR:
		case SENSOR_PARTICLE_TEMPERATURE: 		if (sck_max30105.stop()) return true; break;
		case SENSOR_PM_1:
		case SENSOR_PM_25:
		case SENSOR_PM_10:
		case SENSOR_PN_03:
		case SENSOR_PN_05:
		case SENSOR_PN_1:
		case SENSOR_PN_25:
		case SENSOR_PN_5:
		case SENSOR_PN_10: 				if (sck_pm.stop()) return true; break;
		default: break;
	}

	return false;
}

String SckUrban::getReading(SensorType wichSensor)
{

	switch(wichSensor) {
		case SENSOR_LIGHT:			if (sck_bh1721fvc.get()) return String(sck_bh1721fvc.reading); break;
		case SENSOR_TEMPERATURE: 		if (sck_sht31.update()) return String(sck_sht31.temperature); break;
		case SENSOR_HUMIDITY: 			if (sck_sht31.update()) return String(sck_sht31.humidity); break;
		case SENSOR_CO_RESISTANCE: 		if (sck_mics4514.getCOresistance()) return String(sck_mics4514.coResistance); break;
		case SENSOR_CO_HEAT_VOLT: 		return String(sck_mics4514.getCOheatVoltage()); break;
		case SENSOR_CO_HEAT_TIME:		return String(sck_mics4514.getHeatTime(rtc->getEpoch())); break;
		case SENSOR_NO2_RESISTANCE: 		if (sck_mics4514.getNO2resistance()) return String(sck_mics4514.no2Resistance); break;
		case SENSOR_NO2_HEAT_VOLT: 		return String(sck_mics4514.getNO2heatVoltage()); break;
		case SENSOR_NO2_HEAT_TIME:		return String(sck_mics4514.getHeatTime(rtc->getEpoch())); break;
		case SENSOR_NO2_LOAD_RESISTANCE:	if (sck_mics4514.getNO2load()) return String(sck_mics4514.no2LoadResistor); break;
		case SENSOR_NOISE_DBA: 			if (sck_noise.getReading(SENSOR_NOISE_DBA)) return String(sck_noise.readingDB); break;
		case SENSOR_NOISE_DBC: 			if (sck_noise.getReading(SENSOR_NOISE_DBC)) return String(sck_noise.readingDB); break;
		case SENSOR_NOISE_DBZ: 			if (sck_noise.getReading(SENSOR_NOISE_DBZ)) return String(sck_noise.readingDB); break;
		case SENSOR_NOISE_FFT: 			if (sck_noise.getReading(SENSOR_NOISE_FFT)) {
								// TODO find a way to give access to readingsFFT instead of storing them on a String (too much RAM)
								// For now it just prints the values to console
								for (uint16_t i=1; i<sck_noise.FFT_NUM; i++) SerialUSB.println(sck_noise.readingFFT[i]);
								return "Look above!";
								break;
							}
		case SENSOR_ALTITUDE:			if (sck_mpl3115A2.getAltitude()) return String(sck_mpl3115A2.altitude); break;
		case SENSOR_PRESSURE:			if (sck_mpl3115A2.getPressure()) return String(sck_mpl3115A2.pressure); break;
		case SENSOR_PRESSURE_TEMP:		if (sck_mpl3115A2.getTemperature()) return String(sck_mpl3115A2.temperature); break;
		case SENSOR_PARTICLE_RED:		if (sck_max30105.getRed()) return String(sck_max30105.redChann); break;
		case SENSOR_PARTICLE_GREEN:		if (sck_max30105.getGreen()) return String(sck_max30105.greenChann); break;
		case SENSOR_PARTICLE_IR:		if (sck_max30105.getIR()) return String(sck_max30105.IRchann); break;
		case SENSOR_PARTICLE_TEMPERATURE:	if (sck_max30105.getTemperature()) return String(sck_max30105.temperature); break;
		case SENSOR_PM_1: 			if (sck_pm.update()) return String(sck_pm.pm1); break;
		case SENSOR_PM_25: 			if (sck_pm.update()) return String(sck_pm.pm25); break;
		case SENSOR_PM_10: 			if (sck_pm.update()) return String(sck_pm.pm10); break;
		case SENSOR_PN_03: 			if (sck_pm.update()) return String(sck_pm.pn03); break;
		case SENSOR_PN_05: 			if (sck_pm.update()) return String(sck_pm.pn05); break;
		case SENSOR_PN_1: 			if (sck_pm.update()) return String(sck_pm.pn1); break;
		case SENSOR_PN_25: 			if (sck_pm.update()) return String(sck_pm.pn25); break;
		case SENSOR_PN_5: 			if (sck_pm.update()) return String(sck_pm.pn5); break;
		case SENSOR_PN_10: 			if (sck_pm.update()) return String(sck_pm.pn10); break;
		default: break;
	}

	return "null";
}
bool SckUrban::control(SckBase *base, SensorType wichSensor, String command)
{

         switch (wichSensor) {
		case SENSOR_CO_RESISTANCE:
		case SENSOR_NO2_RESISTANCE: {
			if (command.startsWith("help")) {
				base->sckOut("Available commands for this sensor:\r\nNothing yet!!");
				return true;
			} else base->sckOut("Unrecognized command!! please try again..."); return false;
			break;
		}
		case SENSOR_NOISE_DBA:
		case SENSOR_NOISE_DBC:
		case SENSOR_NOISE_DBZ:
		case SENSOR_NOISE_FFT: {
			if (command.startsWith("debug")) {
				sck_noise.debugFlag = !sck_noise.debugFlag;
				sprintf(base->outBuff, "Noise debug: %s", sck_noise.debugFlag  ? "true" : "false");
				base->sckOut();
				return true;
			}
		}
		default: break;
        }

        base->sckOut("Sensor not recognized!!");
	return false;
}

// Light
bool Sck_BH1721FVC::start()
{
	if (!I2Cdetect(&Wire, address)) return false;
	return true;
}
bool Sck_BH1721FVC::stop()
{
	return true;
}
bool Sck_BH1721FVC::get()
{

	// Datasheet http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1730fvc-e.pdf

	// 0x00 register - CONTROL
	uint8_t CONTROL = B000011;
	// ADC_INTR: 	5	0:Interrupt is inactive.
	// 			1:Interrupt is active.
	// ADC_VALID:	4	0:ADC data is not updated after last reading.
	// 			1:ADC data is updated after last reading.
	// ONE_TIME:	3	0:ADC measurement is continuous.
	// 			1:ADC measurement is one time.
	// 			ADC	transits to power down automatically.
	// DATA_SEL:	2	0:ADC measurement Type0 and Type1.
	// 			1:ADC measurement Type0 only.
	// ADC_EN:	1	0:ADC measurement stop.
	// 			1:ADC measurement start.
	// POWER:	0	0:ADC power down.
	// 			1:ADC power on.

	// 0x01 register - TIMMING
	uint8_t ITIME0  = 0xA0;
	// float TOP = 26500.0; 	 // This is relative to the value above (less resolution more range) TODO define max based on calibration curve (to be implemented)

	// 00h: Start / Stop of measurement is set by special command. (ADC manual integration mode)
	// 01h to FFh: Integration time is determined by ITIME value
	// Integration Time : ITIME_ms = Tint * 964 * (256 - ITIME)
	// Measurement time : Tmt= ITIME_ms + Tint * 714

	// TIME0 posible values, more time = more resolution
	// 0xA0 (~3200 values in 260 ms)
	// 0xB0 (~2100 values in 220 ms)
	// 0xC0 (~1300 values in 180 ms)
	// 0xD0 (~800 values in 130 ms)
	// 0xE0 (~350 values in 88 ms)
	// 0xF0 (~80 values in 45 ms)
	// 0xFA (12 values in 18 ms)
	// 0XFB (8 values in 15 ms)

	// 0x02 register - INTERRUPT
	uint8_t INTERRUPT = B00000000;
	// RES 		7 	Write 0
	// INT_STOP	6	0 : ADC measurement does not stop.
	// 				1 : ADC measurement stops and transits to power down mode when interrupt becomes active.
	// RES 		5 	Write 0
	// INT_EN 	4 	0 : Disable interrupt function.
	// 				1 : Enable interrupt function.
	// PERSIST 	3:0 Interrupt persistence function.
	//	0000 : Interrupt becomes active at each measurement end.
	//	0001 : Interrupt status is updated at each measurement end.
	//	0010 : Interrupt status is updated if two consecutive threshold judgments are the same.
	//	When  set  0011  or  more,  interrupt  status  is  updated  if same threshold judgments continue consecutively same times as the number set in PERSIST.

	// 0x03, 0x04 registers - TH_LOW Low interrupt threshold
	uint8_t TH_LOW0 = 0x00;		// Lower byte of low interrupt threshold
	uint8_t TH_LOW1 = 0x00;		// Upper byte of low interrupt threshold

	// 0x05, 0x06 registers - TH_UP High interrupt threshold
	uint8_t TH_UP0 = 0xFF;		// Lower byte of high interrupt threshold
	uint8_t TH_UP1 = 0xFF;		// Upper byte of high interrupt threshold

	// 0x07 - GAIN
	uint8_t GAIN = 0x00;
	// GAIN  2:0	ADC resolution setting
	//	  X00 : x1 gain mode
	//	  X01 : x2 gain mode
	//	  X10 : x64 gain mode
	//	  X11 : x128 gain mode

	uint8_t DATA[8] = {CONTROL, ITIME0, INTERRUPT, TH_LOW0, TH_LOW1, TH_UP0, TH_UP1, GAIN};

	// Send Configuration
	Wire.beginTransmission(address);
	Wire.write(0x80);
	for (int i= 0; i<8; i++) Wire.write(DATA[i]);
	Wire.endTransmission();


	// TODO calibration curve
	float Tint = 2.8; 	// From datasheet (2.8 typ -- 4.0 max)
	float ITIME_ms = (Tint * 964 * (256 - ITIME0)) / 1000;
	delay (ITIME_ms+50);

	// Ask for reading
	Wire.beginTransmission(address);
	Wire.write(0x94);
	Wire.endTransmission();
	Wire.requestFrom(address, 4);

	// Get result
	int16_t DATA0 = 0;
	uint16_t DATA1 = 0;
	DATA0 = Wire.read();
	DATA0 = DATA0 | (Wire.read()<<8);
	DATA1 = Wire.read();
	DATA1 = DATA1 | (Wire.read()<<8);

	// Setup gain
	uint8_t Gain = 1;
	if (GAIN == 0x01) Gain = 2;
	else if (GAIN == 0x02) Gain = 64;
	else if (GAIN == 0x03) Gain = 128;

	float Lx = 0;
	if (DATA0 !=0) {
	if (DATA1/DATA0 < 0.26) Lx = (1.290 * DATA0 - 2.733 * DATA1) / Gain * 102.6 / ITIME_ms;
	else if (DATA1/DATA0 < 0.55) Lx = (0.795 * DATA0 - 0.859 * DATA1) / Gain * 102.6 / ITIME_ms;
	else if (DATA1/DATA0 < 1.09) Lx = (0.510 * DATA0 - 0.345 * DATA1) / Gain * 102.6 / ITIME_ms;
	else if (DATA1/DATA0 < 2.13) Lx = (0.276 * DATA0 - 0.130 * DATA1) / Gain * 102.6 / ITIME_ms;
	else Lx = 0;
	}

	Lx = max(0, Lx);
	reading  = Lx;

	return true;
}

// SHT31 (Temperature and Humidity)
// TODO Implement calibration routine
// TODO implement heater controller
Sck_SHT31::Sck_SHT31(TwoWire *localWire)
{
	_Wire = localWire;
}
bool Sck_SHT31::start()
{
	_Wire->begin();
	_Wire->beginTransmission(address);
	byte error = _Wire->endTransmission();
	if (error != 0) return false;

	delay(1); 		// In case the device was off
	sendComm(SOFT_RESET); 	// Send reset command
	delay(50); 		// Give time to finish reset
	update();

	return true;
}
bool Sck_SHT31::stop()
{

	// It will go to idle state by itself after 1ms
	return true;
}
bool Sck_SHT31::update()
{
	uint32_t elapsed = millis() - lastTime;
	if (elapsed < timeout) delay(timeout - elapsed);

	uint8_t readbuffer[6];
	sendComm(SINGLE_SHOT_HIGH_REP);

	_Wire->requestFrom(address, (uint8_t)6);
	// Wait for answer (datasheet says 15ms is the max)
	uint32_t started = millis();
	while(_Wire->available() != 6) {
		if (millis() - started > timeout) return 0;
	}

	// Read response
	for (uint8_t i=0; i<6; i++) readbuffer[i] = _Wire->read();

	uint16_t ST, SRH;
	ST = readbuffer[0];
	ST <<= 8;
	ST |= readbuffer[1];

	// Check Temperature crc
	if (readbuffer[2] != crc8(readbuffer, 2)) return false;
	SRH = readbuffer[3];
	SRH <<= 8;
	SRH |= readbuffer[4];

	// check Humidity crc
	if (readbuffer[5] != crc8(readbuffer+3, 2)) return false;
	double temp = ST;
	temp *= 175;
	temp /= 0xffff;
	temp = -45 + temp;
	temperature = (float)temp;

	double shum = SRH;
	shum *= 100;
	shum /= 0xFFFF;
	humidity = (float)shum;

	lastTime = millis();

	return true;
}
void Sck_SHT31::sendComm(uint16_t comm)
{
	_Wire->beginTransmission(address);
	_Wire->write(comm >> 8);
	_Wire->write(comm & 0xFF);
	_Wire->endTransmission();
}
uint8_t Sck_SHT31::crc8(const uint8_t *data, int len)
{

	/* CRC-8 formula from page 14 of SHT spec pdf */

	/* Test data 0xBE, 0xEF should yield 0x92 */

	/* Initialization data 0xFF */
	/* Polynomial 0x31 (x8 + x5 +x4 +1) */
	/* Final XOR 0x00 */

	const uint8_t POLYNOMIAL(0x31);
	uint8_t crc(0xFF);

	for ( int j = len; j; --j ) {
		crc ^= *data++;
		for ( int i = 8; i; --i ) {
			crc = ( crc & 0x80 )
				? (crc << 1) ^ POLYNOMIAL
				: (crc << 1);
		}
	}
	return crc;
}

// Gases
bool Sck_MICS4514::start(uint32_t startTime)
{
	if (!I2Cdetect(&Wire, POT_NO2_LOAD_ADDRESS)) return false;
	if (!I2Cdetect(&Wire, ADC_DIR)) return false;

	if (heaterRunning) return true;

	startHeater();

	// Put the load resistor in middle position
	setNO2load(8000);

	startHeaterTime = startTime;
	return true;
}
bool Sck_MICS4514::stop(uint32_t stopTime)
{
	pinMode(pinPWM_HEATER_CO, OUTPUT);
	pinMode(pinPWM_HEATER_NO2, OUTPUT);
	digitalWrite(pinPWM_HEATER_CO, HIGH);
	digitalWrite(pinPWM_HEATER_NO2, HIGH);

	if (!heaterRunning) return true;
	heaterRunning = false;
	stopHeaterTime = stopTime;
	startHeaterTime = 0;

	TCC1->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
	while (TCC1->SYNCBUSY.reg & TCC_SYNCBUSY_MASK);

	return true;
}
bool Sck_MICS4514::startHeater()
{
	// TODO esto parece ejecutarse dos veces cuando habilitas por primera vez el sensor
	heaterRunning = true;

	// Recalculate heater resistors
	heaterResistance_CO = getCOheatResistance();
	heaterResistance_NO2 = getNO2heatResistance();

	// Recalculate PWM dutyCycles
	dutyCycle_CO = getCOpwm();
	dutyCycle_NO2 = getNO2pwm();

	// Start heaters
	startPWM();

	// Tunne heater voltage
	for (uint8_t i=0; i<3; i++) {
		dutyCycle_CO = getTunnedCOpwm();
		dutyCycle_NO2 = getTunnedNO2pwm();
		startPWM();
	}

	return true;
}
bool Sck_MICS4514::getCOresistance()
{
	float sensorVoltage = getADC(CO_ADC_CHANN);

	if (sensorVoltage == 0) return false;
	if (sensorVoltage > VCC) sensorVoltage = VCC;
	coResistance = (((VCC - sensorVoltage) / sensorVoltage) * coLoadResistor) / 1000.0;
	return true;
}
float Sck_MICS4514::getCOheatVoltage()
{

	return getADC(CO_HEATER_ADC_CHANN)/1000.0;
}
float Sck_MICS4514::getCOpwm()
{
	float desired_voltage = sqrt(CO_HEATING_POWER * heaterResistance_CO);

	// Take in account the series resistor
	float voltageOut = desired_voltage / (heaterResistance_CO / (heaterResistance_CO + heater_seriesResistor));

	// Calculate PWM dutyCycle
	float desiredPWM = voltageOut / heater_VCC * 100;

	return desiredPWM;
}
float Sck_MICS4514::getTunnedCOpwm()
{
	float desiredVoltage = sqrt(CO_HEATING_POWER * heaterResistance_CO);
	float currentVoltage = getCOheatVoltage();

	float diff = (desiredVoltage - currentVoltage) / desiredVoltage;
	float tunnedPWM = (dutyCycle_CO * diff) + dutyCycle_CO;

	return tunnedPWM;
}
float Sck_MICS4514::getCOheatResistance()
{
	// Turn on Heater
	pinMode(pinPWM_HEATER_CO, OUTPUT);
	digitalWrite(pinPWM_HEATER_CO, LOW);

	float heater_voltage = getCOheatVoltage();
	float heater_resistance = heater_voltage / (heater_VCC - heater_voltage) * heater_seriesResistor;

	// Turn off heater
	digitalWrite(pinPWM_HEATER_CO, HIGH);

	return heater_resistance;
}
bool Sck_MICS4514::getNO2resistance()
{
	float sensorVoltage = getADC(NO2_ADC_CHANN);

	if (sensorVoltage == 0) return false;
	if (sensorVoltage > VCC) sensorVoltage = VCC;
	getNO2load();
	no2Resistance = (((VCC - sensorVoltage) / sensorVoltage) * no2LoadResistor) / 1000.0;
	return true;
}
float Sck_MICS4514::getNO2heatVoltage()
{

	return getADC(NO2_HEATER_ADC_CHANN)/1000.0;
}
float Sck_MICS4514::getNO2pwm()
{
	float desired_voltage = sqrt(NO2_HEATING_POWER * heaterResistance_NO2);

	// Take in account the series resistor
	float voltageOut = desired_voltage / (heaterResistance_NO2 / (heaterResistance_NO2 + heater_seriesResistor));

	// Calculate PWM dutyCycle
	float desiredPWM = voltageOut / heater_VCC * 100;

	return desiredPWM;
}
float Sck_MICS4514::getTunnedNO2pwm()
{
	float desiredVoltage = sqrt(NO2_HEATING_POWER * heaterResistance_NO2);
	float currentVoltage = getNO2heatVoltage();

	float diff = (desiredVoltage - currentVoltage) / desiredVoltage;
	float tunnedPWM = (dutyCycle_NO2 * diff) + dutyCycle_NO2;

	return tunnedPWM;
}
float Sck_MICS4514::getNO2heatResistance()
{
	// Turn on Heater
	pinMode(pinPWM_HEATER_NO2, OUTPUT);
	digitalWrite(pinPWM_HEATER_NO2, LOW);

	float heater_voltage = getNO2heatVoltage();
	float heater_resistance = heater_voltage / (heater_VCC - heater_voltage) * heater_seriesResistor;

	// Turn off heater
	digitalWrite(pinPWM_HEATER_NO2, HIGH);

	return heater_resistance;
}
bool Sck_MICS4514::setNO2load(uint32_t value)
{
	// Check minimal/maximal safe value for Gas sensor (Datasheet says 820 Ohms minimal) ~ 870 because of the rounding of POT discrete steps
	if (value < 870) value = 870;
	else if (value > 10000) value = 10000;

	// Data to be writen (based on datasheet page 59) (http://ww1.microchip.com/downloads/en/DeviceDoc/22107B.pdf)
	// Sending data in MSB 7 bits (with a zero at the end)
	byte data = (int)(value / ohmsPerStep);
	data <<= 1;

	const byte MCP4551_CMD_WRITE = 0b00000000;

	Wire.beginTransmission(POT_NO2_LOAD_ADDRESS);
	Wire.write(MCP4551_CMD_WRITE);
	Wire.write(data);
	return (Wire.endTransmission() == 0);
}
bool Sck_MICS4514::getNO2load()
{
	const byte MCP4551_CMD_READ	= 0b00001100;

	Wire.beginTransmission(POT_NO2_LOAD_ADDRESS);
	Wire.write(MCP4551_CMD_READ);
	Wire.endTransmission();
	Wire.requestFrom(POT_NO2_LOAD_ADDRESS, 2);

	// Wait for answer with a timeout
	uint16_t waitTimeout = 500;
	uint32_t time = millis();
	while (!Wire.available()) if ((millis() - time) > waitTimeout) return false;

	// (Based on datasheet page 61) (http://ww1.microchip.com/downloads/en/DeviceDoc/22107B.pdf)
	// using only MSB 7 bits from second byte
	Wire.read();
	byte buffer = Wire.read();
	buffer >>= 1;

	no2LoadResistor = (uint16_t)(buffer * ohmsPerStep);

	return true;
}
bool Sck_MICS4514::startPWM()
{
	// TODO check results at different freq
	// Frequency = GCLK frequency / (2 * N * PER)       where N = prescaler value (CTRLA register)

	// Posible values for N:
	// TCC_CTRLA_PRESCALER_DIV1
	// TCC_CTRLA_PRESCALER_DIV2
	// TCC_CTRLA_PRESCALER_DIV4
	// TCC_CTRLA_PRESCALER_DIV8
	// TCC_CTRLA_PRESCALER_DIV16
	// TCC_CTRLA_PRESCALER_DIV64
	// TCC_CTRLA_PRESCALER_DIV256
	// TCC_CTRLA_PRESCALER_DIV1024
	/* #define MY_DIVIDER TCC_CTRLA_PRESCALER_DIV1 */
	#define MY_DIVIDER TCC_CTRLA_PRESCALER_DIV1024

 	// With N = 1
	// FOR 12 bits
	// Frequency = 48MHz / (2 * 1 * 4096) = 5,859.375 ~ 5.9 * 2 = 11.8 khz
	// Resolution at 5.9kHz = log(4096 + 1) / log(2) = 12 bits
	//
	// FOR 10 bits
	// Frequency = 48MHz / (2 * 1 * 1024) = 23,437.5 ~ 23.4 * 2 = 46.8 khz
	// Resolution at 23.4kHz = log(1024 + 1) / log(2) = 10 bits
	//
	// FOR 8 bits
	// Frequency = 48MHz / (2 * 1 * 256) = 93,750 ~ 93.7 kHz * 2 = 187.4 khz
	// Resolution at 93.7kHz = log(256 + 1) / log(2) = 8 bits
	//
	// FOR 6 bits
	// Frequency = 48MHz / (2 * 1 * 96) = 250 kHz * 2 = 500 khz
	// Resolution at 250kHz = log(96 + 1) / log(2) = 6.6 bits

	uint8_t resolution = 10;					// Resolution in bits
	uint16_t maxValue = pow(2, resolution);

	REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
					  GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
	while (GCLK->STATUS.bit.SYNCBUSY);

	REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
					   GCLK_GENCTRL_GENEN |         // Enable GCLK4
					   GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
					   GCLK_GENCTRL_ID(4);          // Select GCLK4
	while (GCLK->STATUS.bit.SYNCBUSY);

	// Enable the port multiplexer for the digital pin
	PORT->Group[PORTA].PINCFG[8].bit.PMUXEN = 1;
	PORT->Group[PORTA].PINCFG[9].bit.PMUXEN = 1;

	// Connect the TCC0 timer to pin PA09 - port pins are paired odd PMUO and even PMUXE
	// F & E specify the timers: TCC0, TCC1 and TCC2
	PORT->Group[PORTA].PMUX[8 >> 1].reg |= PORT_PMUX_PMUXE_F;
	PORT->Group[PORTA].PMUX[9 >> 1].reg |= PORT_PMUX_PMUXO_F;

	// Feed GCLK4 to TCC0 and TCC1
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
					   GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
					   GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
	while (GCLK->STATUS.bit.SYNCBUSY);

	//Set for Single slope PWM operation: timers or counters count up to TOP value and then repeat
	REG_TCC1_WAVE |=  TCC_WAVE_WAVEGEN_NPWM;
	while (TCC1->SYNCBUSY.bit.WAVE);

	// Each timer counts up to a maximum or TOP value set by the PER register,
	// this determines the frequency of the PWM operation:
	REG_TCC1_PER = maxValue;         		// Set the frequency of the PWM on TCC0
	while (TCC1->SYNCBUSY.bit.PER);

	REG_TCC1_CC1 = (uint16_t)(maxValue * ((100 - dutyCycle_CO) / 100.0)); 	// CO
	REG_TCC1_CC0 = (uint16_t)(maxValue * ((100 - dutyCycle_NO2) / 100.0)); 	// NO2

	while (TCC1->SYNCBUSY.bit.CC1);

	// Divide the 48MHz signal by 1 giving 48MHz (20.83ns) TCC0 timer tick and enable the outputs
	REG_TCC1_CTRLA |= MY_DIVIDER |    // Divide GCLK4 (posibles values: 1,2,4,8,16,64,256,1024)
					  TCC_CTRLA_ENABLE;             // Enable the TCC0 output
	while (TCC1->SYNCBUSY.bit.ENABLE);
	return true;
}
uint32_t Sck_MICS4514::getHeatTime(uint32_t currentTime)
{
	if (!heaterRunning) return 0;
	if (startHeaterTime == 0) startHeaterTime = currentTime;
	return currentTime - startHeaterTime;
}
float Sck_MICS4514::average(uint8_t wichPin)
{

	uint16_t numReadings = 500;
	long total = 0;
	for(uint16_t i=0; i<numReadings; i++) {
		total = total + analogRead(wichPin);
	}
	float average = (float)total / numReadings;
	return average;
}
float Sck_MICS4514::getADC(uint8_t wichChannel)
{
	byte dir[4] = {2,4,6,8};
	byte ask = B11000000 + wichChannel;

	uint32_t result = 0;
	uint8_t numberOfSamples = 20;

	// Average 5 samples
	for (uint8_t i=0; i<numberOfSamples; i++) {
		writeI2C(ADC_DIR, 0, ask);
		writeI2C(ADC_DIR, 0, ask);
		result += (readI2C(ADC_DIR, dir[wichChannel])<<4) + (readI2C(ADC_DIR, dir[wichChannel] + 1)>>4);
	}
	float resultInVoltage = (float)(result / numberOfSamples) * VCC / ANALOG_RESOLUTION;
	return resultInVoltage;
}
void Sck_MICS4514::writeI2C(byte deviceaddress, byte address, byte data )
{
	Wire.beginTransmission(deviceaddress);
	Wire.write(address);
	Wire.write(data);
	Wire.endTransmission();
	delay(4);
}
byte Sck_MICS4514::readI2C(int deviceaddress, byte address)
{
	Wire.beginTransmission(deviceaddress);
	Wire.write(address);
	Wire.endTransmission();
	Wire.requestFrom(deviceaddress,1);
	if (Wire.available() != 1) return 0x00;
	byte data = Wire.read();
	return data;
}


// Noise
bool Sck_Noise::start()
{
	return true;
}
bool Sck_Noise::stop()
{
	return true;
}
bool Sck_Noise::getReading(SensorType wichSensor)
{
	if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, 32)) return false;

	// Wait 263000 I2s cycles or 85 ms at 441000 hz
	uint32_t startPoint = millis();
	while (millis() - startPoint < 100) I2S.read();

	// Fill buffer with samples from I2S bus
	int32_t source[SAMPLE_NUM];
	uint16_t bufferIndex = 0;

	startPoint = millis();
	uint8_t timeOut = 30; 	// (ms) Timeout to avoid hangs if the I2S is not responfing
	while (bufferIndex < SAMPLE_NUM) {
		int32_t buff = I2S.read();
		if (buff) {
			source[bufferIndex] = buff>>7;
			bufferIndex ++;
		}

		if (millis() - startPoint > timeOut) {
			I2S.end();
			return false;
		}
	}
	I2S.end();

	// Get de average of recorded samples
	int32_t sum = 0;
	for (uint16_t i=0; i<SAMPLE_NUM; i++) sum += source[i];
	int32_t avg = sum / SAMPLE_NUM;

	// Center samples in zero
	for (uint16_t i=0; i<SAMPLE_NUM; i++) source[i] = source[i] - avg;

	// FFT
	FFT(source);

	switch(wichSensor) {

		case SENSOR_NOISE_DBA:
			// Equalization and A weighting
			for (uint16_t i=0; i<FFT_NUM; i++) readingFFT[i] *= (double)(equalWeight_A[i] / 65536.0);
			break;
		case SENSOR_NOISE_DBC:
			// Equlization and C weighting
			for (uint16_t i=0; i<FFT_NUM; i++) readingFFT[i] *= (double)(equalWeight_C[i] / 65536.0);
			break;
		case SENSOR_NOISE_DBZ:
			// Just Equalization
			for (uint16_t i=0; i<FFT_NUM; i++) readingFFT[i] *= (double)(equalTab[i] / 65536.0);
			break;
		case SENSOR_NOISE_FFT:
			// Convert FFT to dB
			fft2db();
			return true;
			break;
		default: break;
	}

	// RMS
	long long rmsSum = 0;
	double rmsOut = 0;
	for (uint16_t i=0; i<FFT_NUM; i++) rmsSum += pow(readingFFT[i], 2) / FFT_NUM;
	rmsOut = sqrt(rmsSum);
	rmsOut = rmsOut * 1 / RMS_HANN * sqrt(FFT_NUM) / sqrt(2);

	// Convert to dB
	readingDB = (double) (FULL_SCALE_DBSPL - (FULL_SCALE_DBFS - (20 * log10(rmsOut * sqrt(2)))));

	if (debugFlag) {
		SerialUSB.println("samples, FFT_weighted");
		for (uint16_t i=0; i<SAMPLE_NUM; i++) {
			SerialUSB.print(source[i]);
			SerialUSB.print(",");
			if (i < 256) SerialUSB.println(readingFFT[i]);
			else SerialUSB.println();
		}
	}

	return true;
}
bool Sck_Noise::FFT(int32_t *source)
{
	int16_t scaledSource[SAMPLE_NUM];
	double divider = dynamicScale(source, scaledSource);

	applyWindow(scaledSource, hannWindow, SAMPLE_NUM);

	static int16_t ALIGN4 scratchData[SAMPLE_NUM * 2];

	// Split the data
	for(int i=0; i<SAMPLE_NUM*2; i+=2){
		scratchData[i] = scaledSource[i/2]; // Real
		scratchData[i+1] = 0; // Imaginary
	}

	arm_radix2_butterfly(scratchData, (int16_t)SAMPLE_NUM, (int16_t *)twiddleCoefQ15_512);
	arm_bitreversal(scratchData, SAMPLE_NUM, (uint16_t *)armBitRevTable8);

	for (int i=0; i<SAMPLE_NUM/2; i++) {

		// Calculate result and normalize SpectrumBuffer, also revert dynamic scaling
		uint32_t myReal = pow(scratchData[i*2], 2);
		uint32_t myImg = pow(scratchData[(i*2)+1], 2);

		readingFFT[i] = sqrt(myReal + myImg) * divider * 4;
	}

	// Exception for the first bin
	readingFFT[0] = readingFFT[0] / 2;

	return 0;
}
double Sck_Noise::dynamicScale(int32_t *source, int16_t *scaledSource)
{
	int32_t maxLevel = 0;
	for (uint16_t i=0; i<SAMPLE_NUM; i++) if (abs(source[i]) > maxLevel) maxLevel = abs(source[i]);
	double divider = (maxLevel+1) / 32768.0; // 16 bits
	if (divider < 1) divider = 1;

	for (uint16_t i=0; i<SAMPLE_NUM; i++) scaledSource[i] = source[i] / divider;

	return divider;
}
void Sck_Noise::applyWindow(int16_t *src, const uint16_t *window, uint16_t len)
{
	/* This code is from https://github.com/adafruit/Adafruit_ZeroFFT thank you!
		-------
		This is an FFT library for ARM cortex M0+ CPUs
		Adafruit invests time and resources providing this open source code,
		please support Adafruit and open-source hardware by purchasing products from Adafruit!
		Written by Dean Miller for Adafruit Industries. MIT license, all text above must be included in any redistribution
		------
	*/

	while(len--){
		int32_t val = *src * *window++;
		*src = val >> 15;
		src++;
	}
}
void Sck_Noise::arm_radix2_butterfly(int16_t * pSrc, int16_t fftLen, int16_t * pCoef)
{
	/* This code is from https://github.com/adafruit/Adafruit_ZeroFFT thank you!
		-------
		This is an FFT library for ARM cortex M0+ CPUs
		Adafruit invests time and resources providing this open source code,
		please support Adafruit and open-source hardware by purchasing products from Adafruit!
		Written by Dean Miller for Adafruit Industries. MIT license, all text above must be included in any redistribution
		------
	*/

	int i, j, k, l;
	int n1, n2, ia;
	int16_t xt, yt, cosVal, sinVal;

	n2 = fftLen;

	n1 = n2;
	n2 = n2 >> 1;
	ia = 0;

	// loop for groups
	for (j=0; j<n2; j++) {
		cosVal = pCoef[ia * 2];
		sinVal = pCoef[(ia * 2) + 1];
		ia++;

		// loop for butterfly
		for (i=j; i<fftLen; i+=n1) {
			l = i + n2;
			xt = (pSrc[2 * i] >> 2u) - (pSrc[2 * l] >> 2u);
			pSrc[2 * i] = ((pSrc[2 * i] >> 2u) + (pSrc[2 * l] >> 2u)) >> 1u;

			yt = (pSrc[2 * i + 1] >> 2u) - (pSrc[2 * l + 1] >> 2u);
			pSrc[2 * i + 1] =
				((pSrc[2 * l + 1] >> 2u) + (pSrc[2 * i + 1] >> 2u)) >> 1u;

			pSrc[2u * l] = (((int16_t) (((int32_t) xt * cosVal) >> 16)) +
					((int16_t) (((int32_t) yt * sinVal) >> 16)));

			pSrc[2u * l + 1u] = (((int16_t) (((int32_t) yt * cosVal) >> 16)) -
					((int16_t) (((int32_t) xt * sinVal) >> 16)));

		}                           // butterfly loop end
	}                             // groups loop end

	uint16_t twidCoefModifier = 2;

	// loop for stage
	for (k = fftLen / 2; k > 2; k = k >> 1) {
		n1 = n2;
		n2 = n2 >> 1;
		ia = 0;

		// loop for groups
		for (j=0; j<n2; j++) {
			cosVal = pCoef[ia * 2];
			sinVal = pCoef[(ia * 2) + 1];

			ia = ia + twidCoefModifier;

			// loop for butterfly
			for (i=j; i<fftLen; i+=n1) {
				l = i + n2;
				xt = pSrc[2 * i] - pSrc[2 * l];
				pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]) >> 1u;

				yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
				pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]) >> 1u;

				pSrc[2u * l] = (((int16_t) (((int32_t) xt * cosVal) >> 16)) +
						((int16_t) (((int32_t) yt * sinVal) >> 16)));

				pSrc[2u * l + 1u] = (((int16_t) (((int32_t) yt * cosVal) >> 16)) -
						((int16_t) (((int32_t) xt * sinVal) >> 16)));

			}                         // butterfly loop end
		}                           // groups loop end
		twidCoefModifier = twidCoefModifier << 1u;
	}                             // stages loop end

	n1 = n2;
	n2 = n2 >> 1;
	ia = 0;
	// loop for groups
	for (j=0; j<n2; j++) {
		cosVal = pCoef[ia * 2];
		sinVal = pCoef[(ia * 2) + 1];

		ia = ia + twidCoefModifier;

		// loop for butterfly
		for (i=j; i<fftLen; i+=n1) {
			l = i + n2;
			xt = pSrc[2 * i] - pSrc[2 * l];
			pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]);

			yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
			pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]);

			pSrc[2u * l] = xt;

			pSrc[2u * l + 1u] = yt;

		}                           // butterfly loop end
	}                             // groups loop end
}
void Sck_Noise::arm_bitreversal(int16_t * pSrc16, uint32_t fftLen, uint16_t * pBitRevTab)
{
	/* This code is from https://github.com/adafruit/Adafruit_ZeroFFT thank you!
		-------
		This is an FFT library for ARM cortex M0+ CPUs
		Adafruit invests time and resources providing this open source code,
		please support Adafruit and open-source hardware by purchasing products from Adafruit!
		Written by Dean Miller for Adafruit Industries. MIT license, all text above must be included in any redistribution
		------
	*/

	int32_t *pSrc = (int32_t *) pSrc16;
	int32_t in;
	uint32_t fftLenBy2, fftLenBy2p1;
	uint32_t i, j;

	/*  Initializations */
	j = 0u;
	fftLenBy2 = fftLen / 2u;
	fftLenBy2p1 = (fftLen / 2u) + 1u;

	/* Bit Reversal Implementation */
	for (i = 0u; i <= (fftLenBy2 - 2u); i += 2u) {
		if(i < j) {
			in = pSrc[i];
			pSrc[i] = pSrc[j];
			pSrc[j] = in;

			in = pSrc[i + fftLenBy2p1];
			pSrc[i + fftLenBy2p1] = pSrc[j + fftLenBy2p1];
			pSrc[j + fftLenBy2p1] = in;
		}

		in = pSrc[i + 1u];
		pSrc[i + 1u] = pSrc[j + fftLenBy2];
		pSrc[j + fftLenBy2] = in;

		/*  Reading the index for the bit reversal */
		j = *pBitRevTab;

		/*  Updating the bit reversal index depending on the fft length  */
		pBitRevTab++;
	}
}
void Sck_Noise::fft2db()
{
    for (uint16_t i=0; i<FFT_NUM; i++) {
	    if (readingFFT[i] > 0) readingFFT[i] = FULL_SCALE_DBSPL - (FULL_SCALE_DBFS - (20 * log10(readingFFT[i] * sqrt(2))));
	    if (readingFFT[i] < 0) readingFFT[i] = 0;
    }
}

// Barometric pressure and Altitude
bool Sck_MPL3115A2::start()
{
	if (!I2Cdetect(&Wire, address)) return false;
	if (Adafruit_mpl3115A2.begin()) return true;
	return false;
}
bool Sck_MPL3115A2::stop()
{

	return true;
}
bool Sck_MPL3115A2::getAltitude()
{

	Adafruit_mpl3115A2.begin();

	// TODO callibration with control interface
	// Maybe we could implement get online data to calibrate this
	// mpl3115A2.setSeaPressure(102250.0);

	// TODO timeout to prevent hangs on external lib
	altitude = Adafruit_mpl3115A2.getAltitude();

	return true;
}
bool Sck_MPL3115A2::getPressure()
{

	Adafruit_mpl3115A2.begin();

	// TODO timeout to prevent hangs on external lib
	pressure = Adafruit_mpl3115A2.getPressure() / 1000;

	return true;
}
bool Sck_MPL3115A2::getTemperature()
{

	Adafruit_mpl3115A2.begin();

	// TODO timeout to prevent hangs on external lib
	altitude = Adafruit_mpl3115A2.getAltitude();
	temperature =  Adafruit_mpl3115A2.getTemperature();	// Only works after a getAltitude! don't call this allone

	return true;
}

// Dust Particles
bool Sck_MAX30105::start()
{
	if (!I2Cdetect(&Wire, address)) return false;
	if (sparkfun_max30105.begin()) return true;
	return false;
}
bool Sck_MAX30105::stop()
{

	sparkfun_max30105.shutDown();
	return true;
}
bool Sck_MAX30105::getRed()
{

	sparkfun_max30105.wakeUp();

	// TODO Dig more into setup parameters
	sparkfun_max30105.setup();
	redChann = (float)sparkfun_max30105.getRed();
	sparkfun_max30105.shutDown();
	return true;
}
bool Sck_MAX30105::getGreen()
{

	sparkfun_max30105.wakeUp();

	// TODO Dig more into setup parameters
	sparkfun_max30105.setup();
	greenChann = (float)sparkfun_max30105.getGreen();
	sparkfun_max30105.shutDown();
	return true;
}
bool Sck_MAX30105::getIR()
{

	sparkfun_max30105.wakeUp();

	// TODO Dig more into setup parameters
	sparkfun_max30105.setup();
	IRchann = (float)sparkfun_max30105.getIR();
	sparkfun_max30105.shutDown();
	return true;
}
bool Sck_MAX30105::getTemperature()
{
	// NOT WORKING!!! (sparkfun lib)

	sparkfun_max30105.wakeUp();

	// TODO Dig more into setup parameters
	sparkfun_max30105.setup(0);
	temperature = (float)sparkfun_max30105.readTemperature();
	sparkfun_max30105.shutDown();
	return true;
}

// PM sensor
bool Sck_PM::start()
{
	if (started) return true;
	if (detectionFailed) return false;

	pinMode(pinPM_ENABLE, OUTPUT);
	digitalWrite(pinPM_ENABLE, HIGH);
	SerialPM.begin(9600);

	uint32_t startTimer = millis();
	while (millis() - startTimer < 4000) {
		delay(50);
		if (SerialPM.available()) {
			started = true;
			return true;
		}
	}
	stop();
	detectionFailed = true;
	return false;
}
bool Sck_PM::stop()
{
	digitalWrite(pinPM_ENABLE, LOW);
	SerialPM.end();
	started = false;
	detectionFailed = false;

	return true;
}
bool Sck_PM::update()
{
	if (millis() - lastReading < 1000) return true; 	// PM sensor only delivers one reading per second
	if (millis() - lastFail < 1000) return false;

	// Empty serial buffer
	while(SerialPM.available()) SerialPM.read();

	// Wait for new readings
	uint32_t startPoint = millis();
	while(SerialPM.available() < 25) {
		if (millis() - startPoint > 1000) {
			// Timeout
			lastFail = millis();
			return false;
		}
	}

	while(SerialPM.available()) {

		byte sb = 0;
		sb = SerialPM.read();

		if (sb == 0x42) {
			SerialPM.readBytes(buff, buffLong);
			if (buff[0] == 0x4d) {

				pm1 = (buff[3]<<8) + buff[4];
				pm25 = (buff[5]<<8) + buff[6];
				pm10 = (buff[7]<<8) + buff[8];
				pn03 = (buff[15]<<8) + buff[16];
				pn05 = (buff[17]<<8) + buff[18];
				pn1 = (buff[19]<<8) + buff[20];
				pn25 = (buff[21]<<8) + buff[22];
				pn5 = (buff[23]<<8) + buff[24];
				pn10 = (buff[25]<<8) + buff[26];

				lastReading = millis();
				lastFail = 0;

				return true;
			}
		}
	}
	return false;
}
bool Sck_PM::reset()
{
	digitalWrite(pinPM_ENABLE, LOW);
	delay(200);
	digitalWrite(pinPM_ENABLE, HIGH);
	return true;
}
