# hw.dll regions without a source file yet

Ranges of hw.dll code that tools/align_linux.py could not attribute (no Linux counterpart or no anchors), identified by the strings they use. Addresses are approximate run bounds from progress.csv rows with an empty source_file (2026-10-17).

## Engine code (Valve, in scope)

| Range | Size | What | Evidence |
|---|---|---|---|
| 01D07248-01D0EDA5 | 30 KB | vgui2 HTML control (Internet Explorer host) | "VGUI_HTML", "Internet Explorer_Server", "MemoryBitmap" |
| 01D534E9-01D5A601 | 27 KB | mouse input (in_win.c) and the download cache (cl_parsefn.c or a Windows-only file) | "-noforcemparms", "-noforcemspd", "DownloadCache", "%s/overflow" |
| 01D5A723-01D5C307 | 7 KB | resource verification and download for GameUI | "#GameUI_VerifyingAndDownloading" |
| 01D88030-01D895CF | 5 KB | no strings; between engine files | |
| 01D8AC40-01D8CE32 | 9 KB | mathlib (BoxOnPlaneSide) and nearby code | "BoxOnPlaneSide:  Bad signbits", "cstrike" |
| 01D96A50-01DA0822 | 40 KB | the OpenGL-on-Direct3D wrapper, GL_VIDNT.C and the overlay hook | "Wrapper: Lock on primary failed", "GL_SGIS_multitexture", "Software\\Microsoft\\Quake", "GameOverlayRenderer.dll" |
| 01DD0B23-01DD4A7B | 16 KB | snd_win.c (DirectSound, EAX) | "dsound.dll", "DirectSoundCreate", "DS:CreateSoundBuffer for EAX front left" |
| 01DF759A-01DF9267 | 6 KB | video mode and registry settings (sys_dll2, vid) | "EngineDLL", "EngineD3D", "ScreenWindowed", "ScreenBPP" |
| 01DFD212-01DFF007 | 7 KB | vgui2 engine surface | "EngineSurface007" |
| 01E0BD4E-01E0E52B | 9 KB | voice recording through waveIn and DirectSoundCapture | "VoiceRecord_WaveIn::Init", "DirectSoundCaptureCreate" |
| 01E2D83E-01E305A9, 01E322A0-01E33D43, 01E43E40-01E4580D and more | | vgui2 controls library (Panel, Button, RichText, ...), also in GameUI.dll | "vgui::", "DefaultButtonSet", "DoCutSelected" |

## Third-party code linked into hw.dll (no Valve source)

| Range | Size | What | Evidence |
|---|---|---|---|
| 01D0EE19-01D1E6BE, 01E1A0C0-01E1FBF5, 01E20060-01E27F5A | 110 KB | IGA / Radial in-game advertising SDK | "RAD_IMPRESSION_SENT", "Software\\IGA", "radial.cdb", "FINAL 2.0.0.0" |
| 01E14E50-01E18431 | 8 KB | Aureal A3D SDK | "Software\\Aureal\\A3D", "CA3DRenderer::SetMainVolume" |
| 01E634F8-01E6FD98 | 47 KB | bzip2 1.0.2 (official source; built with the VC6 processor pack, docs/toolchains.md) | "1.0.2, 30-Dec-2001" |
| 01E83AC5-01E899EF | 22 KB | DirectX manager (ddraw enumeration, COM registration) | "__DXMgrUsageCount__", "DirectDrawEnumerateExA" |

How to treat the third-party code (rebuild from a source where one exists, as with bzip2, or link the retail bytes as a binary object) is a scope decision that is still open.
