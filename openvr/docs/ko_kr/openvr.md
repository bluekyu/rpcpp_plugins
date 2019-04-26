# OpenVR
**Languages**: [English](../openvr.md)

## 개요
OpenVR (https://github.com/ValveSoftware/openvr) 라이브러리를 위한 플러그인이다. OpenVR 플러그인이 활성화 되어 있으면,
플러그인 로드 단계에서 OpenVR 를 초기화 한다. 그리고 OpenVRRenderStage 에서 마지막에 그려진 화면을 OpenVR texture 에 복사하고,
매 프레임 마지막 단계에서 해당 texture 를 HMD 에 보낸다.

## 사용법
Render Pipeline C++ 의 [스테레오 및 가상현실](https://github.com/bluekyu/render_pipeline_cpp/blob/master/docs/ko_kr/rendering/stereo-and-vr.md) 문서 참고.

## 내부 기능 및 설정
### 거리 단위 및 렌더링 스케일
OpenVR 에서는 미터 단위를 사용하므로, 렌더링 단위와 맞지 않을 경우 스케일링이 필요하다.
플러그인에서는 config 설정에 있는 `distance_scale` 설정을 사용하여 이를 자동으로 조절한다.
예를 들어 기본값은 5 이고, 1 meter 는 렌더링 시에 5 유닛으로 사용된다.

코드 상에서는 `OpenVRPlugin::set_distance_scale` 함수를 통해서 조절이 가능하고,
이 값은 카메라 노드의 위치 조절과 "device_node_group" 노드의 스케일 값에 반영된다.
따라서 각 device node 들은 OpenVR 의 원래 값을 그대로 사용하므로 미터 단위를 가지며, 월드 좌표계에서
`distance_scale` 만큼 스케일링 된다.

### 해상도 및 Supersample 조절
OpenVR 렌더링 해상도는 OpenVR 에서 권장하는 해상도를 사용한다.
SteamVR 1522617861 이후 버전부터 GPU 성능을 고려하여 해상도를 자동으로 조절한다.
예를 들면, NVIDIA GTX 1080 Ti 에서는 200% 비율을 가져서 `2138 x 2376` 해상도를 권장 해상도로 제공한다.

OpenVR 플러그인에서는 설정 파일(plugins.yaml 파일)을 통해 조절할 수 있도록 3가지 모드를 제공한다.

#### Supersample 모드 및 Supersample 비율 설정
`supersample_mode` 설정(string)을 이용하여, `auto`, `force`, `ignore` 모드가 존재하고 기본값은 `auto` 이다.
그리고 각 모드에서는 `supersample_scale` 값(float)을 이용하여 Supersample 비율을 조절한다. (기본값은 1.0)

`auto` 에서는 OpenVR 해상도 비율을 사용하되, `supersample_scale` 을 곱한 값을 사용한다.
예를 들어, 2배 (200%) 비율에 `supersample_scale` 값이 0.7 라면, 1.4배 (140%) 비율을 이용한다.
이를 이용하여, 어플리케이션에서 렌더링 리소스가 많이 필요할 경우, 권장 해상도보다 작은 값을 이용하도록 설정할 수 있다.

`force` 에서는 OpenVR 해상도 비율에 `supersample_scale` 의 값을 강제로 사용한다.
만일, 이전 SteamVR 에서 사용하던 고정 해상도 방식을 사용하려면 `force` 모드에서 `supersample_scale` 1.0 값을 설정하면 된다.

`ignore` 에서는 권장 해상도 값을 무시하고, Render Pipeline 에서 사용하는 해상도를 그대로 적용한다.
즉, `pipeline.yaml` 파일에서 지정하는 해상도 값들을 이용한다.

#### 최소 Supersample 비율 설정
`supersample_scale_min` 설정(float)을 이용하여 `auto` 및 `force` 모드에서 최소 비율을 지정할 수 있다. (기본값은 1.0)
예를 들어, `auto` 모드에서 `supersample_scale` 적용할 경우, 저사양 GPU 에서는 1.0 미만으로 조절이 될 수 있는데,
이를 방지할 수 있다.



## 최적화

### CPU 및 GPU 리소스 배분
OpenVR 에서는 (Vive의 경우) 90Hz 의 동기화를 필요로 하는데, 동기화를 수행하는 시점은 OpenVR 장치들의 위치를
새로 구하는 시점(`vr::IVRCompositor::WaitGetPoses`) 이다.
이 시점에서 이전에 submit 한 텍스처가 완료되기를 기다리며, 작업이 끝나면 새로운 위치를 계산해서 넘겨준다.

OpenVR 플러그인에서는 -60 sort 값(`OpenVRPlugin::UPDATE_TASK_SORT`)을 갖는
`OpenVRPlugin::Impl::wait_get_poses` 단계에서 이를 수행한다.
따라서 게임 로직이 -60 이후에 수행된다면, wait_get_poses 시점에서
약 4~5 ms (Render Pipeline 플러그인들의 활성화 여부에 따라 다름) 정도의 대기(stall)가 발생한다.

```
    +----------+---------+--------------+---------+----------+
CPU | T1 (N+1) | S (N+1) |   T2 (N+1)   | R (N+1) | T1 (N+2) |
    +----------+---------+--------------+---------+----------+
                         ↑                    ↘
    +--------------------+---------------+--------------------+
GPU |        R (N)       |               |       R (N+1)      |
    +--------------------+---------------+--------------------+
```
(GPU의 N 프레임 렌더링이 끝나기 전까지 CPU 에서는 대기(S)가 발생한다.)

참고로, 일반적인 Panda3D 에서 게임 로직을 렌더링에 반영하는 시점(동기화 시점)은 50 sort 값을 갖는 `task_igloop` 의
초기 단계에서 수행된다. (즉, SwapBuffers 시점)
그러므로 GPU 렌더링이 수행되는 동안 게임 로직을 업데이트할 수 있는 여유 시간을 가질 수 있다.

```
    +------------------------+---------+------------------------+
CPU |         T (N+1)        | R (N+1) |         T (N+2)        |
    +------------------------+---------+------------------------+
                         ↑        ↘
    +--------------------+----+--------------------+
GPU |        R (N)       |    |        R (N+1)     |
    +--------------------+----+--------------------+
```
(GPU의 N 프레임 렌더링 동안 전까지 CPU 에서 task (T)를 처리할 수 있다.)

만일, OpenVR 장치들의 위치를 직접 필요로 하는 부분(절대 좌표 등)이 없다면, 이러한 작업들을 -50 sort 이전으로
설정하면 VR 렌더링 동안 CPU 작업을 처리할 수 있도록 자원을 배분할 수 있다.



## OpenVR SDK Tips
### 트래킹 없을 때에도 화면 출력하기
`steamvr.vrsettings` 파일에서 `forceFadeOnBadTracking : false` 값을  steamvr 섹션에 추가하거나,
OpenVR API 를 사용하여 해댱 섹션 값을 설정하면 됨.
```cpp
vr::VRSettings()->SetBool(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_ForceFadeOnBadTracking_Bool, false);
```
