#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <RTCZero.h>

#include <Sensors.h>
#include "Pins.h"
#include "MAX30105.h"
#include <Adafruit_MPL3115A2.h>
#include "SckSoundTables.h"
#include <I2S.h>

// Firmware for SmartCitizen Kit - Urban Sensor Board SCK 2.0
// It includes drivers for this sensors:
//
// * Light - BH1721 -> (0x29)
// * Temperature and Humidity - SHT31 -> (0x44)
// * CO and NO2 - MICS4515
//      digital POT -> 0x2F
// 	ADS7924 MICSADC -> 0x48
// * Noise  - Invensense ICS43432 I2S microphone;microphone:
// * Barometric pressure - MPL3115 -> (0x60)
// * Dust Particles - MAX30105 -> (0x57)

// Pins
const uint8_t pinPWM_HEATER_CO = pinBOARD_CONN_3;
const uint8_t pinPWM_HEATER_NO2 = pinBOARD_CONN_5;
const uint8_t pinPM_SERIAL_RX = pinBOARD_CONN_9;
const uint8_t pinPM_SERIAL_TX = pinBOARD_CONN_11;
const uint8_t pinPM_ENABLE = pinBOARD_CONN_7; 		// HIGH Enable PMS power

enum SensorState
{
	SENSOR_BUSY,
	SENSOR_READY,
	SENSOR_ERROR
};

// Light
class Sck_BH1721FVC
{
	// Datasheet
	// http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1721fvc-e.pdf

	private:
		bool sendCommand(byte wichCommand);
		byte readRegister(byte wichRegister);

	public:
		uint8_t address = 0x29;
		float reading;
		bool start();
		bool stop();
		bool get();
};

// Temperature and Humidity
class Sck_SHT31
{
	// Datasheet
	// https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf
	// This code is based on Adafruit SHT31 library, thanks! (https://github.com/adafruit/Adafruit_SHT31)
	private:

		TwoWire *_Wire;

		// Commands
		const uint16_t SOFT_RESET = 0x30A2;
		const uint16_t SINGLE_SHOT_HIGH_REP = 0x2400;

		uint32_t timeout = 15;	// Time in ms to wait for a reading
		uint32_t lastTime = 0;
		void sendComm(uint16_t comm);
		uint8_t crc8(const uint8_t *data, int len);
	public:
		uint8_t address = 0x44;
		// Conntructor
		Sck_SHT31(TwoWire *localWire);

		float temperature;
		float humidity;
		bool start();
		bool stop();
		bool update();
};

// Gases CO and NO2
class Sck_MICS4514
{
	// Datasheet
	// http://files.manylabs.org/datasheets/MICS-4514.pdf
	// TODO create a generic class and instantiate CO, NO2
	private:

		const float heater_VCC = 3.235; 		// (volts) Measured on Hex inverter IC1
		const float heater_seriesResistor = 10.0;

		// Carbon Monoxide
		float heaterResistance_CO = 74.0; 		// Nominal value from datasheet (will be recalculated on boot)
		float dutyCycle_CO = 50.0;  			// Start with low power until we can get the Heater resistance value.
		const uint8_t CO_HEATER_ADC_CHANN = 3;
		const uint8_t CO_ADC_CHANN = 2;
		const float CO_HEATING_POWER = 0.076; 		// (watts) Typical heating power from datasheet

		// Nitrogen Dioxide
		float heaterResistance_NO2 = 66.0; 		// Nominal value from datasheet (will be recalculated on boot)
		float dutyCycle_NO2 = 50.0;  			// Start with low power until we can get the Heater resistance value.
		const uint8_t NO2_HEATER_ADC_CHANN = 1;
		const uint8_t NO2_ADC_CHANN = 0;
		const float NO2_HEATING_POWER = 0.043;  		// (watts) Typical heating power from datasheet

		const uint32_t ANALOG_RESOLUTION = 4095;
		const uint32_t VCC = 3278; 			// (mV) Measured manually on MICS input
		byte ADC_DIR = 0x48;

		bool heaterRunning = false;
		uint32_t startHeaterTime = 0;
		uint32_t stopHeaterTime = 0;

		// CO Fixed resistor
		uint32_t coLoadResistor = 750000;

		// NO2 adjustable load resistor
		const byte POT_NO2_LOAD_ADDRESS = 0x2F;
		const float ohmsPerStep	= 10000.0/127; // Ohms for each potenciometer step

		void writeI2C(byte deviceaddress, byte address, byte data );
		byte readI2C(int deviceaddress, byte address);

	public:
		float coResistance;
		float no2Resistance;
		uint16_t no2LoadResistor;
		bool start(uint32_t startTime);
		bool stop(uint32_t stopTime);
		bool startHeater();
		bool startPWM();
		bool getCOresistance();
		float getCOheatVoltage();
		float getCOpwm();
		float getTunnedCOpwm();
		float getCOheatResistance();
		bool getNO2resistance();
		float getNO2heatVoltage();
		float getNO2pwm();
		float getTunnedNO2pwm();
		float getNO2heatResistance();
		bool setNO2load(uint32_t value);
		bool getNO2load();
		uint32_t getHeatTime(uint32_t currentTime);
		float average(uint8_t wichPin);
		float getADC(uint8_t wichChannel);
};

// Noise
class Sck_Noise
{
	private:
		const double RMS_HANN = 0.61177;
		const uint8_t FULL_SCALE_DBSPL = 120;
		const uint8_t BIT_LENGTH = 24;
		const double FULL_SCALE_DBFS = 20*log10(pow(2,(BIT_LENGTH)));
		bool FFT(int32_t *source);
		void arm_bitreversal(int16_t * pSrc16, uint32_t fftLen, uint16_t * pBitRevTab);
		void arm_radix2_butterfly( int16_t * pSrc, int16_t fftLen, int16_t * pCoef);
		void applyWindow(int16_t *src, const uint16_t *window, uint16_t len);
		double dynamicScale(int32_t *source, int16_t *scaledSource);
		void fft2db();

	public:
		bool debugFlag = false;
		const uint32_t sampleRate = 44100;
		static const uint16_t SAMPLE_NUM = 512;
		static const uint16_t FFT_NUM = 256;
		float readingDB;
		int32_t readingFFT[FFT_NUM];
		bool start();
		bool stop();
		bool getReading(SensorType wichSensor);

};

// Barometric pressure and Altitude
class Sck_MPL3115A2
{
	// Datasheet
	// https://cache.freescale.com/files/sensors/doc/data_sheet/MPL3115A2.pdf

	private:
		Adafruit_MPL3115A2 Adafruit_mpl3115A2 = Adafruit_MPL3115A2();

	public:
		uint8_t address = 0x60;
		float altitude;
		float pressure;
		float temperature;
		bool start();
		bool stop();
		bool getAltitude();
		bool getPressure();
		bool getTemperature();
};

// Dust Particles
class Sck_MAX30105
{
	// Datasheet
	// https://datasheets.maximintegrated.com/en/ds/MAX30105.pdf

	private:
		MAX30105 sparkfun_max30105;

	public:
		uint8_t address = 0x57;
		float redChann;
		float greenChann;
		float IRchann;
		float temperature;
		bool start();
		bool stop();
		bool getRed();
		bool getGreen();
		bool getIR();
		bool getTemperature();	// NOT WORKING!!! (sparkfun lib)
};

//PM sensors
class Sck_PM
{
	private:
		bool detectionFailed = false;
		uint32_t lastFail = 0;
		uint32_t lastReading = 0;

							// 12 bytes:
							// 0:1->pm1, 2:3->pm25, 4:5->pm10,
							// 6:7->pm1, 8:9->pm2.5, 10:11->pm10   (under atmosferic enviroment)
							// Number of particles with diameter beyond X um in 0.1 L of air.
							// 12:13 -> 0.3 um
							// 14:15 -> 0.5 um
							// 16:17 -> 1.0 um
							// 18:19 -> 2.5 um
							// 20:21 -> 5.0 um
							// 22:23 -> 10.0 um

		static const uint8_t buffLong = 27;
		unsigned char buff[buffLong];
		uint32_t rtcStarted = 0;
		uint32_t rtcStopped = 0;
		uint32_t rtcReading = 0;
		RTCZero* rtc;

	public:
		Sck_PM(RTCZero* myrtc) {
			rtc = myrtc;
		}
		// Readings
		uint16_t pm1;
		uint16_t pm25;
		uint16_t pm10;
		uint16_t pn03;
		uint16_t pn05;
		uint16_t pn1;
		uint16_t pn25;
		uint16_t pn5;
		uint16_t pn10;

		bool started = false;
		uint16_t oneShotPeriod = 15;

		bool start();
		bool stop();
		bool update();
		int16_t oneShot(uint16_t period);
		bool reset();
};

class SckBase;

class SckUrban
{
	private:
		struct Resistor {
			byte deviceAddress;
			byte resistorAddress;
		};
		RTCZero* rtc;

	public:
		SckUrban(RTCZero* myrtc) {
			rtc = myrtc;
		}

		bool present();
		bool setup();
		bool start(SensorType wichSensor);
		bool stop(SensorType wichSensor);

		// String getReading(); https://stackoverflow.com/questions/14840173/c-same-function-parameters-with-different-return-type
		void getReading(OneSensor *wichSensor);
		bool control(SckBase *base, SensorType wichSensor, String command);

		// Light
		Sck_BH1721FVC sck_bh1721fvc;

		// Temperature and Humidity
		Sck_SHT31 sck_sht31 = Sck_SHT31(&Wire);

		// Gases CO and NO2
		Sck_MICS4514 sck_mics4514;

		// Noise
		Sck_Noise sck_noise;

		// Barometric pressure and Altitude
		Sck_MPL3115A2 sck_mpl3115A2;

		// Dust Particles
		Sck_MAX30105 sck_max30105;

		// PM sensor
		Sck_PM sck_pm = Sck_PM(rtc);
};

