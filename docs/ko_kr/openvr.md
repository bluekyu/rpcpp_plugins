# OpenVR
**Translation**: [English](../openvr.md)

## 개요
OpenVR (https://github.com/ValveSoftware/openvr) 라이브러리를 위한 플러그인이다. OpenVR 플러그인이 활성화 되어 있으면,
플러그인 로드 단계에서 OpenVR 를 초기화 한다. 그리고 OpenVRRenderStage 에서 마지막에 그려진 화면을 OpenVR texture 에 복사하고,
매 프레임 마지막 단계에서 해당 texture 를 HMD 에 보낸다.

## 사용법
Render Pipeline C++ 의 [스테레오 및 가상현실](https://github.com/bluekyu/render_pipeline_cpp/blob/master/docs/ko_kr/rendering/stereo-and-vr.md) 문서 참고.
