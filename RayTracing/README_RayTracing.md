Ray Tracing

VC++ 2015

input: RayTracing.exe -input [input_filename] -output [output_filename] -size 300 300 -bounces 4

[input_filename] may be one of the txt files, such as scene01_plane.txt . [output\_filename] is any bmp, such as output1b.bmp

output: creates or writes output_filename. 

a4soln.exe, a5soln.exe are the solutions from the MIT OCW. I think a4soln only works with scenes 01-05 and 07-09. The images I've generated are those that don't end in a or b. The others are from the solutions. I have to admit that Scenes 1-9 were from a previous Ray Casting program, so I might not have updated everything correctly.

Condensed design notes: Uses recursive ray tracing, the Phong model of materials, possibly some textures, supersampling/convolution/down-sampling for anti-aliasing
