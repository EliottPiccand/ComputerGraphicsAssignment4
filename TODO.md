# TODO
## Requirements
- ~~radar as opaque red cone + cylinder at the back~~
- ~~cannonballs parabolic trajectories~~
- ~~water as triangle quad mesh~~
- ~~turret view (keyboard update the ship, no turret motion)~~
- ~~cannonball view (above and beyond the cannonball, facing forward)~~
- ~~at least 2 enemies~~
    - ~~move~~
    - ~~shoot~~
- ~~hp system~~
    - ~~24k hp~~
    - ~~health display~~
    - ~~damages 3k-15k randomly~~
- ~~victory/defeat menu~~
    - ~~victory state~~
- ~~collisions~~
- ~~switch to shaders and remove any trace of OpenGL legacy pipeline~~
- ~~add rendering style: Wireframe rendering with hidden line removal~~ (no hidden line removal for anything that disable depth test such as particles and skybox)
- ~~find something to make cannon balls visible from top view~~
- ~~fix non textured model rendering~~
- ~~fix explosion particles~~
- ~~fix message transparency~~
- Gouraud shading mode (cycle with 'g')
- Phong shading mode
- light points:
    - island firecamp/lantern
    - cannonballs
- moving sunlight
- toggle textures with 't'
- toggle normal maps with 'n'
- water texture mapping
- update Report
    - content
    - screenshots
- update README
    - Controls
    - Compile info

## Additional Goals
- ~~explosion effect~~
- ~~camera shake (on getting hit)~~
- ~~foam trail~~
- ~~cannon recoil~~
- ~~flapping flag~~
- speed dependent propeller
- motion blur
- shadows

## Custom Additional Goals
- ~~water splash on cannonball hit water~~
- ~~sparks on cannon ball stem~~
- ~~red overlay on getting hit~~
- ~~instanciated rendering for particle system~~
- ~~smoke when cannon fire~~
- ~~pbr~~
- ~~waves (https://alextardif.com/Water.html)~~
- fireworks on win
- sink animation
- rotating ship wheel
- moving rudder
- sun
- birds
- water interaction

## If time
- ~~move textures in the assets/texture folder and fix models accordingly~~
- ~~update every part using Instant or Duration to something not linked to the system clock, to properly be able to pause the game.~~
- ~~make particle spawning events and add them to debug scene~~
- fix copy_asset.py when ???
- readd debug view
- debug GameObject names
