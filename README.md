# 3D-Engine
SDL computer graphics engine in C that aims to implemented 3D using Linear Algebra and 90's style programming.


<h4>Version 1.5</h4>
<h6>
  Renderer can now read and construct objects from .obj files (e.g. Blender). <br>
  Lighting is also slighly improved.
</h6>
<div>
  <img src="https://github.com/user-attachments/assets/6b3b97ff-5d1a-4ed2-a57c-453b0d7b7ffa" alt="mountains" width=700/>
</div>

<h4>Version 1.4<h4>
<h6>
  Incorporates real 3D calculations using object space, world space, and camera space. 
  <br>
  This is combined with a complete 3D rendering pipeline consisting of multiple stages:
  <ul>
    <li>Object culling</li>
    <li>Object local-to-world transformation</li>
    <li>Remove Backfaces</li>
    <li>Lighting</li>
    <li>Object world-to-view transformation</li>
    <li>Object clipping</li>
    <li>Generate Polygon list</li>
    <li>Clip polygons</li>
    <li>Draw polygons</li>
  </ul>
  
  Renderer allows for reading of objects as PLG files and simple vector text files.
</h6>
<div>
  <img src="https://github.com/user-attachments/assets/23734d29-b830-4f2f-8893-805d757e430d" alt="cubes" width="700"/>
</div>

<h4>Version 1.3</h4>
<h6>Untextured Raycaster that casts rays on a 2D outlined map to compute distance to objects. <br>
  The height of each object can thus be determined by the distance the ray travels before reaching it, <br>
  the further the distance the smaller the object. Also referred to as "fake 3D" or "2.5D". 
</h6>

<div>
  <!--<img src="https://github.com/user-attachments/assets/e7fee9e5-2e0c-40b2-8c8a-dcfd3b987b1f" alt="raycaster1" width="500"/>-->
  <img src="https://github.com/user-attachments/assets/8e8829bf-bd1f-4c47-bc4b-3b09ae424f3c" alt="raycaster2" width="500"/>
</div>

<h4>Version 1.2</h4>
<h6>Rotating Wireframe 3D
cube built in the 1.2 version using matrix multiplication and world to screen projection.</h6>

<img src="https://github.com/leolangberg/C-Engine/assets/152855963/cfbef467-5c20-415c-959d-262c47cbde8c" alt="spincube" width="300"/>
<!-- ![7baa876c974f41e6fa91beec1f88f7b8-ezgif com-video-to-gif-converter](https://github.com/leolangberg/C-Engine/assets/152855963/cfbef467-5c20-415c-959d-262c47cbde8c) -->











<!-- ![raycaster1](https://github.com/user-attachments/assets/e7fee9e5-2e0c-40b2-8c8a-dcfd3b987b1f) -->
<!-- ![raycaster2](https://github.com/user-attachments/assets/8e8829bf-bd1f-4c47-bc4b-3b09ae424f3c) -->

<h5>
  Source References:
  <h6>
    <ul>
      <li>A. Lamothe, "Tricks of the Windows Game Programming Gurus | Fundamentals of 2D and 3D Game Programming", (1999, Oct). </li>
      <li>A. Lamothe, "Black Art of 3D Game Programming | Writing Your Own High-Speed 3D Polygon Video Games in C", (1995, Sep 1).</li> 
      <li>L. Vandevenne, <i>Lode's Computer Graphics Tutorial: Raycasting</i>. Lodev.org. https://lodev.org/cgtutor/raycasting.html</li>
      <li>"Programming a first person shooter from scratch like it's 1995", jdh.</li>
      <li>"Code-It-Yourself! 3D Graphics Engine", javidx9</li>
    </ul>
  </h6>
</h5>
