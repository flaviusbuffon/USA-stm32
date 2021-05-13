#define FILTER 1
#include "..\CONFIGURATION.h"

/*һͨ�����ֵ�ͨ�˲���
dt:�˲�ʱ������С��   tau:�˲�ʱ�䳣����
���������������ͬһ������e.g. Angle
Increment Ϊ������
Angle = a*Angle +(1-a)Increment
*/
float ch1_Float_LowPass_Filter(float data,float Increment,float dt,float tau)
{
	float a = tau/(tau+dt);
	//printf("\ninput= %f,inc=%f,a=%f,LPF:%f",data,Increment,a,(data*a+(1-a)*Increment));
	return (data*a+(1-a)*Increment);
}


void filter_IMU_data(u16 freq)
{
	
	acceFilter.channelX = pAcceleration.channelX;
	acceFilter.channelY = pAcceleration.channelY;
	acceFilter.channelZ = pAcceleration.channelZ;
	
	pAcceleration.channelX = eAcceleration.channelX;
	pAcceleration.channelY = eAcceleration.channelY;
	pAcceleration.channelZ = eAcceleration.channelZ;
	//���ٶȼ�Low pass
	pAcceleration.channelX = ch1_Float_LowPass_Filter(acceFilter.channelX,pAcceleration.channelX-acceFilter.channelX,\
																										(float)(1/(float)freq),0.05);
	pAcceleration.channelY = ch1_Float_LowPass_Filter(acceFilter.channelY,pAcceleration.channelY-acceFilter.channelY,\
																										(float)(1/(float)freq),0.05);
	pAcceleration.channelZ = ch1_Float_LowPass_Filter(acceFilter.channelZ,pAcceleration.channelZ-acceFilter.channelZ,\
																										(float)(1/(float)freq),0.05);
	
	//���ٶ�ֵ��ȥGyro�ľ���
	
	anglespeedFilter.channelX = pAngleSpeed.channelX;
	anglespeedFilter.channelY = pAngleSpeed.channelY;
	anglespeedFilter.channelZ = pAngleSpeed.channelZ;
	
	pAngleSpeed.channelX = eAngleSpeed.channelX - Gyro_Cancelation.channelX;
	pAngleSpeed.channelY = eAngleSpeed.channelY - Gyro_Cancelation.channelY;
	pAngleSpeed.channelZ = eAngleSpeed.channelZ - Gyro_Cancelation.channelZ;
	//Low pass
	pAngleSpeed.channelX = ch1_Float_LowPass_Filter(anglespeedFilter.channelX,pAngleSpeed.channelX-anglespeedFilter.channelX,\
																										(float)(1/(float)freq),0.05);
	pAngleSpeed.channelY = ch1_Float_LowPass_Filter(anglespeedFilter.channelY,pAngleSpeed.channelY-anglespeedFilter.channelY,\
																										(float)(1/(float)freq),0.05);
	pAngleSpeed.channelZ = ch1_Float_LowPass_Filter(anglespeedFilter.channelZ,pAngleSpeed.channelZ-anglespeedFilter.channelZ,\
																										(float)(1/(float)freq),0.05);	
	
	
	pCompass_Main.channelX = (eCompass_Main.channelX - Compass_Hard_Iron_Cancellation.channelX);
	pCompass_Main.channelY = (eCompass_Main.channelY - Compass_Hard_Iron_Cancellation.channelY);
	pCompass_Main.channelZ = (eCompass_Main.channelZ - Compass_Hard_Iron_Cancellation.channelZ);
// 	printf("MAG X%d Y%d Z%d\n",eCompass_Main.channelX,eCompass_Main.channelY,eCompass_Main.channelZ);
}
