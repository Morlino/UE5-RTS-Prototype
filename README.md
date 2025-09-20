# RTS Prototype (UE5 C++)

This is a portfolio project: a minimal RTS inspired by StarCraft 2.  
The focus is on **C++ gameplay systems** and **Unreal Engine 5 skills**, not polished assets.  
The goal is to showcase unit systems, AI, resource management, and basic PvP mechanics.

# Project Progress:

### Phase 1: Setup & Foundation
- [x] Create UE5 Blank C++ project
- [x] Setup folder structure (`Source/`, `Content/`, etc.)
- [x] Implement basic GameMode & PlayerController
- [+] Spawn a simple placeholder unit (cube or capsule)
- [x] Implement basic movement (click-to-move or camera controls)

### Phase 2: Unit System
- [x] Unit selection (single & multiple)
- [+] Unit movement via NavMesh
- [ ] Unit stats: health, damage, attack range
- [ ] Basic attack mechanic (target enemy → deal damage)
- [ ] Unit death handling

### Phase 3: Resource System
- [ ] Resource nodes (placeholder spheres/cubes)
- [ ] Worker units
- [ ] Resource collection
- [ ] Resource storage & UI display
- [ ] Resource deduction for units/buildings

### Phase 4: Building System
- [ ] Building placement (grid-based or free)
- [ ] Construction progress timer
- [ ] Unit production queue
- [ ] Link resources to building/unit creation

### Phase 5: AI Opponent
- [ ] Implement AIController for enemy units
- [ ] Basic AI tasks: gather → build → attack
- [ ] Target selection logic
- [ ] Simple AI build orders (scripted)

### Phase 6: UI & HUD
- [ ] Resource counter
- [ ] Unit/building selection panel
- [ ] Unit/building command buttons
- [ ] Highlight selected units/buildings

### Phase 7: Polishing & Optional Multiplayer
- [ ] Simple VFX/SFX placeholders
- [ ] Attack/death effects using Niagara
- [ ] Clean up C++ architecture (modular, clean code)
- [ ] Optional: basic network replication
- [ ] Test single-player PvP with AI
- [ ] Record short gameplay video for portfolio
