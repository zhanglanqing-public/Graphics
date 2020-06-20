#pragma once
#include <windows.h>
#include "Math.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods


#define	FORWARD 0
#define BACKWARD 1
#define LEFT 2
#define RIGHT 3


// Default camera values， 这个开始的视角是写死的，因此下面不能随便更改eye的初始位置
const float YAW = 225.0f;
const float PITCH = -45.0f;
const float SPEED = 0.1f;
const float SENSITIVTY = 0.5f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	Vec Position;
	Vec Front;
	Vec Right;
	Vec WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(Vec position = Vec(2.5f, 2.5f, 2.5f), Vec up = Vec(0.0f, 0.0f, 1.0f), float yaw = YAW, float pitch = PITCH) :
		MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}
	

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void Move(int direction)
	{
		float velocity = this->MovementSpeed;
		if (direction == FORWARD)
			this->Position = this->Position + this->Front * velocity;
		if (direction == BACKWARD)
			this->Position = this->Position - this->Front * velocity;
		// TODO: direction bug?
		if (direction == RIGHT)
			this->Position = this->Position - this->Right * velocity;
		if (direction == LEFT)
			this->Position = this->Position + this->Right * velocity;
	}


	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void Eye_move(int direction, boolean constrainPitch = true)
	{
		
		if (direction == LEFT) {
			this->Yaw -= MouseSensitivity;
		} else if (direction == RIGHT) {
			this->Yaw += MouseSensitivity;
		} else if (direction == FORWARD) {
			this->Pitch += MouseSensitivity;
		} else {
			this->Pitch -= MouseSensitivity;
		}

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch) {
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	Vec Eye_position() { return this->Position; }
	
	Vec Look_at_position() { return this->Position + this->Front; }
	
	
private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		Vec front;
		front.z = sin(radians(this->Pitch));
		front.x = cos(radians(this->Yaw)) * cos(radians(this->Pitch));
		front.y = sin(radians(this->Yaw)) * cos(radians(this->Pitch));
		this->Front = front;
		this->Front.Nomalize();
		// Also re-calculate the Right and Up vector
		this->Right = cross(this->Front, this->WorldUp);
		this->Right.Nomalize();
	}

	float radians(float angle) { return angle * PI / 180.0; }
};