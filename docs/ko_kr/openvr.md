# OpenVR
**Translation**: [English](../openvr.md)

## 개요
OpenVR (https://github.com/ValveSoftware/openvr) 라이브러리를 위한 플러그인이다. OpenVR 플러그인이 활성화 되어 있으면,
플러그인 로드 단계에서 OpenVR 를 초기화 한다. 그리고 OpenVRRenderStage 에서 마지막에 그려진 화면을 OpenVR texture 에 복사하고,
매 프레임 마지막 단계에서 해당 texture 를 HMD 에 보낸다.

## 사용법
Render Pipeline C++ 의 [스테레오 및 가상현실](https://github.com/bluekyu/render_pipeline_cpp/blob/master/docs/ko_kr/rendering/stereo-and-vr.md) 문서 참고.

## 내부 기능
### 거리 단위 및 렌더링 스케일
OpenVR 에서는 미터 단위를 사용하므로, 렌더링 단위와 맞지 않을 경우 스케일링이 필요하다.
플러그인에서는 config 설정에 있는 `distance_scale` 설정을 사용하여 이를 자동으로 조절한다.
예를 들어 기본값은 5 이고, 1 meter 는 렌더링 시에 5 유닛으로 사용된다.

코드 상에서는 `OpenVRPlugin::set_distance_scale` 함수를 통해서 조절이 가능하고,
이 값은 카메라 노드의 위치 조절과 "device_node_group" 노드의 스케일 값에 반영된다.
따라서 각 device node 들은 OpenVR 의 원래 값을 그대로 사용하므로 미터 단위를 가지며, 월드 좌표계에서
distance_scale 만큼 스케일링 된다.
