## Explanation of parameters

* **Cycle settings** This cycles throught the presets, using `wait seconds` and `distance threshold` to determine when to move on the next (randomly chosen) preset
* **maxSpeed** the maximum speed the particles can move at
* **maxForce** you probably don't want to change that
* **cohesionDistance** & **separationDistance** the distance at which each particle will want to stick to/repel each other
* **cohesionAmount** & **separationAmount** the amount each particle want to repel/stick to each other
* **do flock** make the particles react to each other!
* **followAmount** the amount at which particles want to follow the current path
* **move along** move the particles along the path
* **alpha** the lower this value the longer the trails are
* **imageSize** the size of each particle
* **distanceThreshold** If the particle is less than this distance from its target it's considered arrived
* **wait seconds** the number of seconds before it switches to the next preset
* **path index** the path to follow, indices mean:
	* 0: TMRW logo
	* 1: Message in `message.txt`
	* 2: Current time!
	* 3: Low res IcoSphere
	* 4: Archimedian Sphere
	

## Controls Keys

* **s**: save the current preset (this will open a dialog box)
* **space bar**: show/hide the GUI
* **f**: toggle fullscreen


## Global Settings

Edit the `data/base_settings.xml` file for things like:

* Number of agents (dots) in the simulation
* Width & Height of the window
* Colour palette filename (and if it should cycle or choose randomly)
* The font filename & size



