# 🚀 AvalonR

This Project is a space combat game where your ship and crew fight through procedurally generated waves of enemies across dynamic maps in deep space. Upgrade your firepower, customize your loadout, and survive the relentless onslaught!

![Screenshot Placeholder](https://via.placeholder.com/800x400?text=AvalonR+Gameplay)

---

## 🛠️ Getting Started

### Clone the repository:

```bash
git clone https://github.com/pratybos/project-AvalonR.git
cd project-AvalonR
```
Initialize submodules:
```bash
git submodule update --init --recursive
```
Build the Game:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Rebuild(after making changes):
```bash
ninja
```

--------------------------------------------------

In Game, you control an upgradable spaceship and must survive 6 levels, each with 3 waves of enemies. With each cleared wave, you grow stronger by selecting upgrades and buying gear with collected coins.

🌌 Features:
Randomized Maps: Levels are picked from a pool of unique maps each time you play.

--------------------------------------------------

Enemy Generation: Adaptive enemy spawns challenge your tactics every wave. With currently 3 enemy types each with their unique strategy to fight the Player.

--------------------------------------------------

Ship Upgrades:

Crew Upgrades (selected during level-ups):

Movement speed

Reload speed

Damage per bullet

Passive HP regeneration

--------------------------------------------------

Store Upgrades (purchased using in-game currency):

Fire Modes:

Shotgun

Radial(Spiral)

Parallel

Core Upgrades:

Increased HP

Healing items

More bullets per magazine

More damage per shot

Additional projectiles

--------------------------------------------------

🧠 Strategy & Progression
Each level is more dangerous than the last — planning your upgrades is key. Build a loadout that suits your playstyle:

Go aggressive with multi-shot upgrades

Focus on survivability with regen and HP boosts

Mix and match for a balanced, reactive build

--------------------------------------------------

📦 Dependencies
CMake

C++ -> 20+

Ninja (for faster builds)

The Dependencies are baked into the game 

--------------------------------------------------

📜 License
MIT License

--------------------------------------------------

🚀 Screenshots & Media
(Coming soon!)
