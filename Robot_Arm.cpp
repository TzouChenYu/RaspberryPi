#include "Robot_Arm.h"

Robot_Arm::Robot_Arm()
{
	Robot_DOF = 0;

	Set_Ini_Theta_Flag = false;

	T_Base2Global = Eigen::Matrix4d::Identity();

	Ini_P.resize(6); // 6*1
	P.resize(6);     // 6*1
}

Robot_Arm::Robot_Arm(int DOF)
{
	Robot_DOF = DOF;

	Set_Ini_Theta_Flag = false;

	T_Base2Global = Eigen::Matrix4d::Identity();

	TransFormation_Matrix.reserve(DOF+1);

	Ini_P.resize(6); // 6*1
	P.resize(6);     // 6*1
}

Robot_Arm::~Robot_Arm()
{

}

int Robot_Arm::Get_RobotDOF()
{
	return Robot_DOF;
}


void Robot_Arm::Set_Base2Globalframe(Eigen::Matrix4d& T)
{
	T_Base2Global = T;
}

void Robot_Arm::Set_Ini_Theta(double *Ini_theta)
{
	// �̧ǱNTF Matrix ��Jvector��
	if(!Set_Ini_Theta_Flag && Robot_DOF != 0)
	{
		TransFormation_Matrix.push_back( DH2TransforMatrix(72.75, -90, 24.3, Ini_theta[0]) ); //T01
		TransFormation_Matrix.push_back( DH2TransforMatrix(150.35, 0, 0, Ini_theta[1]) * DH2TransforMatrix(0, -90, 0, -90) ); //T12
		TransFormation_Matrix.push_back( DH2TransforMatrix(0, -90, 145.35, Ini_theta[2]) ); //T23
		TransFormation_Matrix.push_back( DH2TransforMatrix(260, 0, 0, Ini_theta[3]) ); //T34
		TransFormation_Matrix.push_back( DH2TransforMatrix(120, 0, 0, Ini_theta[4]) ); //T45
		
		T_BaseEnd = Eigen::Matrix4d::Identity();
		
		for(int i=0; i < TransFormation_Matrix.size(); i++)
		{
			T_BaseEnd *= TransFormation_Matrix[i];
		}
		
		// T_BaseEnd = T_Base2Global * T05
		T_BaseEnd = T_Base2Global * T_BaseEnd;

		Get_Robot_PosOri(T_BaseEnd, Ini_P);

		P = Ini_P; // �w�]��l�۵�

		Set_Ini_Theta_Flag = true;
	}
}

Eigen::Matrix4d Robot_Arm::DH2TransforMatrix(double a, double alpha, double d, double theta)
{
	alpha = Deg2Rad(alpha);
    theta = Deg2Rad(theta);
    
	Eigen::Matrix4d T;

   T << cos(theta), -sin(theta)*cos(alpha),  sin(theta)*sin(alpha),   a*cos(theta),
        sin(theta),  cos(theta)*cos(alpha),  -cos(theta)*sin(alpha),  a*sin(theta),
        0,           sin(alpha),             cos(alpha),              d,
        0,           0,                      0,                       1;

	return T;
}

void Robot_Arm::DH2TransforMatrix(double a, double alpha, double d, double theta, Eigen::Matrix4d& T)
{
	alpha = Deg2Rad(alpha);
    theta = Deg2Rad(theta);
    
   T << cos(theta), -sin(theta)*cos(alpha),  sin(theta)*sin(alpha),   a*cos(theta),
        sin(theta),  cos(theta)*cos(alpha),  -cos(theta)*sin(alpha),  a*sin(theta),
        0,           sin(alpha),             cos(alpha),              d,
        0,           0,                      0,                       1;

}

void Robot_Arm::Refresh_TFMatrix(double *New_theta)
{
	// ��sTF Matrix
	DH2TransforMatrix(72.75, -90, 24.3, New_theta[0], TransFormation_Matrix[0]); //T01
	TransFormation_Matrix[1] = DH2TransforMatrix(150.35, 0, 0, New_theta[1]) * DH2TransforMatrix(0, -90, 0, -90); //T12
	DH2TransforMatrix(0, -90, 145.35, New_theta[2], TransFormation_Matrix[2]); //T23
	DH2TransforMatrix(260, 0, 0, New_theta[3], TransFormation_Matrix[3]); //T34
	DH2TransforMatrix(120, 0, 0, New_theta[4], TransFormation_Matrix[4]); //T45

	T_BaseEnd = Eigen::Matrix4d::Identity();

	for(int i=0; i < TransFormation_Matrix.size(); i++)
	{
		T_BaseEnd *= TransFormation_Matrix[i];
	}
		
	// T_BaseEnd = T_Base2Global * T05
	T_BaseEnd = T_Base2Global * T_BaseEnd;

	Get_Robot_PosOri(T_BaseEnd, P);
}


void Robot_Arm::Get_Robot_PosOri(Eigen::Matrix4d& T, Eigen::VectorXd & _P)
{
	Eigen::Matrix3d R = T.topLeftCorner(3,3);

	_P << T(0,3), T(1,3), T(2,3), Get_Orientation(R);
}

Eigen::Vector3d Robot_Arm::Get_Orientation(Eigen::Matrix3d& R)
{
	Eigen::Vector3d k;

	double theta = acos( (R(0,0)+R(1,1)+R(2,2)-1)/2 );

	// theta = 0 (deg) ==> singular
	if(abs(theta) <= 0.0000001)
	{
		k << 0, 0, 0;
	}
	else if(abs(theta-pi) <= 0.0000001) // theta = 180 (deg) ==> singular
	{
		Eigen::Matrix3d temp = (R + Eigen::Matrix3d::Identity()) / 2;
		
		// �O kx >= 0
		k(0) = sqrt(temp(0,0));

		if(k(0) == 0)
		{
			if(temp(2,1) > 0) // ky*kz > 0
			{
				k(1) = sqrt(temp(1,1)); //ky
				k(2) = sqrt(temp(2,2)); //kZ
			}
			else
			{
				k(1) = sqrt(temp(1,1)); //ky
				k(2) = -sqrt(temp(2,2)); //kZ
			}
		}
		else  // kx > 0
		{
			if(temp(1,0) > 0) // kx*ky > 0
				k(1) = sqrt(temp(1,1)); //ky
			else
				k(1) = -sqrt(temp(1,1)); //ky


			if(temp(2,0) > 0) // kx*kz > 0
				k(2) = sqrt(temp(2,2)); //kz
			else
				k(2) = -sqrt(temp(2,2)); //kz
		}
	}
	else
	{
		k << R(2,1)-R(1,2), R(0,2)-R(2,0), R(1,0)-R(0,1);
		k = k / (2*sin(theta));
	}

	return theta*k;
}