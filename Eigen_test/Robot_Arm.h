#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "/usr/include/Eigen_3.2.7/Eigen/Dense"


#define pi 3.14159265359
#define Deg2Rad(theta) (theta*pi/180) 
#define Rad2Deg(theta) (theta*180/pi)


class Robot_Arm
{
private:
	int Robot_DOF;

	// �qGlobal Frame ��� Base Frame ==> �w�]�O Identity Matrix
	Eigen::Matrix4d T_Base2Global;
	
	bool Set_Ini_Theta_Flag;

	// �x�s��l���A�����I��[Position Orientation]�A�HBase frame���
	Eigen::VectorXd Ini_P;

	std::vector<Eigen::Matrix4d> TransFormation_Matrix;

	// �qRotation Matrix����o���u��Orientation
	Eigen::Vector3d Get_Orientation(Eigen::Matrix3d& R);

	// �qTransformation Matrix����o���u��[Position Orientation]
	void Get_Robot_PosOri(Eigen::Matrix4d& T, Eigen::VectorXd & _P);

public:
	// �غc�l
	Robot_Arm();

	// �]�w�ۥѫ�
	Robot_Arm(int DOF);


	// �Ѻc�l
	~Robot_Arm();

	int Get_RobotDOF();

	// �]�w Global Frame ��� Base Frame �� TF_Matrix�A�w�]�OIdentity
	void Set_Base2Globalframe(Eigen::Matrix4d& T);

	// �]�w��l����(deg)
	void Set_Ini_Theta(double *Ini_theta);


	// ����TransFormation Matrix�A���(mm, deg, mm, deg)
	Eigen::Matrix4d DH2TransforMatrix(double a, double alpha, double d, double theta);

	void DH2TransforMatrix(double a, double alpha, double d, double theta, Eigen::Matrix4d& T);

	
	// ��s �Ҧ��ۥѫפ�theta��(deg) ==> ��sTF Matrix, P
	void Refreah_TFMatrix(double *New_theta);

	
	// �o��{�b�����I�۹�Base��TF Matrix
	Eigen::Matrix4d T_BaseEnd;

	// �N�{�b�����I��TF Matrix ���Ħ� [Position Orientation]�A�HBase frame���
	Eigen::VectorXd P;
};