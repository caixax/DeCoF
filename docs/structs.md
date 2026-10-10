# Struct and class layouts that differ from the HLSDK

Only layouts confirmed in the binary go here. Each entry names the binary,
the evidence and the method.

## entvars_t (hl.dll 1.6a): 4 bytes inserted after light_level

Confirmed 2026-10-10 from the save/restore table `gEntvarsDescription`
(102 TYPEDESCRIPTION records of 16 bytes: fieldType, fieldName,
fieldOffset, fieldSize, flags).

| Field | HLSDK / CoF 1.55 offset | CoF 1.6a offset |
|---|---|---|
| gravity | 0x11C | 0x11C |
| light_level | 0x124 | 0x124 |
| *(unnamed, 4 bytes, not saved)* | - | 0x128 |
| sequence | 0x128 | 0x12C |
| frame | 0x130 | 0x134 |
| health | 0x160 | 0x164 |
| flags | 0x1A4 | 0x1A8 |
| iuser1 | 0x244 | 0x248 |
| euser4 (last saved field) | 0x2A0 | 0x2A4 |
| sizeof(entvars_t) | 0x2A4 | 0x2A8 |

- 1.55 table: VA 0x10216B40 (PDB symbol `gEntvarsDescription`), offsets
  identical to the stock HLSDK. The 2012 builds (and so the PDB type
  information) use the stock layout.
- 1.6a table: VA 0x1021AAE0 (file offset 0x219AE0). Every field from
  `sequence` on is shifted by +4.
- The new field is absent from the save table, so its name and type are
  unknown. Placeholder name until evidence appears: `guess_light_level_pad`.
- Consequence: `edict_t` grows to at least 0x328 in 1.6a. cof-enhanced
  reports an edict stride of 0x32C, which would mean a second 4-byte change
  elsewhere in `edict_t`; not yet verified.
