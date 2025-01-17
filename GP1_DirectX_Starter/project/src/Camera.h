#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

#include <algorithm>



namespace dae {
	class Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		};

		float aspectRatio{ 1 };
		Vector3 origin{};

		Vector3 forward{ Vector3::UnitZ };

	private:
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix ProjectionMatrix{};

		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };


		float totalPitch{};
		float totalYaw{};

		float movementSpeed{ 10 };
		float mouseSensivity{ 5 };
		float mouseUpSpeed{ 250 };
		double rendererMultiplier{ 1.0 }; // if it is directX should be 1 if it is cpu should be lower


		float zn{ 1.f };
		float zf{ 100.f };

		bool bIsDirectX{ true };

	public:
		Matrix GetViewMatrix() const { return viewMatrix; }
		Matrix GetProjectionMatrix() const { return ProjectionMatrix; }


		void Initialize(float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f })
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//ONB => invViewMatrix
			//Inverse(ONB) => ViewMatrix

			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			invViewMatrix =
			{
				{right.x,right.y,right.z,0},
				{up.x,up.y,up.z,0},
				{forward.x,forward.y,forward.z,0},
				{origin.x,origin.y,origin.z,1}
			};

			viewMatrix = Matrix::Inverse(invViewMatrix);
			//viewMatrix = Matrix::CreateLookAtLH(origin,forward,up);


			//printf("%f %f %f \n", forward.x, forward.y, forward.z);
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			//TODO W3

			ProjectionMatrix = Matrix::CreatePerspectiveFovLH(fov, aspectRatio, zn, zf);

			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Camera Update Logic
			//...
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (pKeyboardState[SDL_SCANCODE_F1])
			{
				bIsDirectX = !bIsDirectX;
			}

			if (bIsDirectX)
			{
				rendererMultiplier = 1;

			}
			else
			{
				rendererMultiplier = 0.001;
			}

			

			if (mouseState & SDL_BUTTON(3))
			{

				totalPitch -= mouseY * deltaTime * mouseSensivity * rendererMultiplier;
				totalYaw += mouseX * deltaTime * mouseSensivity * rendererMultiplier;
				totalPitch = std::clamp(totalPitch, -PI_DIV_2 + FLT_EPSILON, PI_DIV_2 - FLT_EPSILON);
				forward = Matrix::CreateRotation(totalPitch, totalYaw, 0).TransformVector(Vector3::UnitZ).Normalized();



				if (pKeyboardState[SDL_SCANCODE_W])
				{
					origin += (forward * deltaTime * movementSpeed);
				}
				if (pKeyboardState[SDL_SCANCODE_S])
				{
					origin -= forward * deltaTime * movementSpeed ;
				}
				if (pKeyboardState[SDL_SCANCODE_A])
				{
					origin -= right * deltaTime * movementSpeed ;
				}
				if (pKeyboardState[SDL_SCANCODE_D])
				{
					origin += right * deltaTime * movementSpeed ;
				}
				if (pKeyboardState[SDL_SCANCODE_Q])
				{
					origin -= up * deltaTime * movementSpeed ;
				}
				if (pKeyboardState[SDL_SCANCODE_E])
				{
					origin += up * deltaTime * movementSpeed ;
				}
			}
			if (mouseState & SDL_BUTTON(1))
			{
				origin -= mouseY * up * deltaTime * mouseUpSpeed * rendererMultiplier;
				origin += mouseX * right * deltaTime * mouseUpSpeed * rendererMultiplier;

			}
			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}
