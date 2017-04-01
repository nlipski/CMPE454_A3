# uncomment the -lpng12 and -DHAVEPNG if you have the png12 libraries installed on Linux.

PROG = rt

OBJS =	main.o window.o arcballWindow.o font.o scene.o sphere.o triangle.o light.o eye.o object.o \
	material.o texture.o vertex.o wavefrontobj.o wavefront.o linalg.o gpuProgram.o photonmap.o axes.o

INCS = 
LIBS = -lpthread -lGL -lGLU -lglut -lGLEW # -lpng12

CXXFLAGS = -g $(INCS) -Wno-write-strings -Wno-unused-result -DLINUX -pthread # -DHAVEPNG
CXX      = g++

$(PROG):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS) $(LIBS)

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o *~ core anim Makefile.bak

depend:	
	makedepend -Y *.h *.cpp

# DO NOT DELETE

arcballWindow.o: headers.h linalg.h window.h
axes.o: linalg.h gpuProgram.h headers.h
eye.o: linalg.h
gpuProgram.o: headers.h linalg.h
light.o: linalg.h
main.o: seq.h scene.h linalg.h object.h material.h texture.h headers.h
main.o: gpuProgram.h light.h eye.h photonmap.h priority.h rtWindow.h
main.o: arcballWindow.h window.h
material.o: linalg.h texture.h headers.h seq.h
object.o: linalg.h material.h texture.h headers.h seq.h gpuProgram.h
photonmap.o: headers.h seq.h linalg.h priority.h
rtWindow.o: arcballWindow.h headers.h linalg.h window.h scene.h seq.h
rtWindow.o: object.h material.h texture.h gpuProgram.h light.h eye.h
rtWindow.o: photonmap.h priority.h
scene.o: seq.h linalg.h object.h material.h texture.h headers.h gpuProgram.h
scene.o: light.h eye.h photonmap.h priority.h
sphere.o: object.h linalg.h material.h texture.h headers.h seq.h gpuProgram.h
texture.o: headers.h seq.h linalg.h
triangle.o: object.h linalg.h material.h texture.h headers.h seq.h
triangle.o: gpuProgram.h vertex.h
vertex.o: linalg.h
wavefront.o: headers.h seq.h linalg.h shadeMode.h gpuProgram.h
wavefrontobj.o: object.h linalg.h material.h texture.h headers.h seq.h
wavefrontobj.o: gpuProgram.h wavefront.h shadeMode.h
window.o: headers.h linalg.h
arcballWindow.o: headers.h arcballWindow.h linalg.h window.h font.h
axes.o: headers.h axes.h linalg.h gpuProgram.h
eye.o: headers.h eye.h linalg.h main.h seq.h scene.h object.h material.h
eye.o: texture.h gpuProgram.h light.h photonmap.h priority.h rtWindow.h
eye.o: arcballWindow.h window.h
font.o: headers.h
gpuProgram.o: gpuProgram.h headers.h linalg.h
light.o: headers.h light.h linalg.h main.h seq.h scene.h object.h material.h
light.o: texture.h gpuProgram.h eye.h photonmap.h priority.h rtWindow.h
light.o: arcballWindow.h window.h
linalg.o: linalg.h
main.o: headers.h rtWindow.h arcballWindow.h linalg.h window.h scene.h seq.h
main.o: object.h material.h texture.h gpuProgram.h light.h eye.h photonmap.h
main.o: priority.h
material.o: headers.h material.h linalg.h texture.h seq.h main.h scene.h
material.o: object.h gpuProgram.h light.h eye.h photonmap.h priority.h
material.o: rtWindow.h arcballWindow.h window.h
object.o: headers.h object.h linalg.h material.h texture.h seq.h gpuProgram.h
object.o: main.h scene.h light.h eye.h photonmap.h priority.h rtWindow.h
object.o: arcballWindow.h window.h
photonmap.o: photonmap.h headers.h seq.h linalg.h priority.h scene.h object.h
photonmap.o: material.h texture.h gpuProgram.h light.h eye.h priority.cpp
priority.o: priority.h
scene.o: headers.h scene.h seq.h linalg.h object.h material.h texture.h
scene.o: gpuProgram.h light.h eye.h photonmap.h priority.h rtWindow.h
scene.o: arcballWindow.h window.h sphere.h triangle.h vertex.h wavefrontobj.h
scene.o: wavefront.h shadeMode.h font.h main.h
sphere.o: headers.h sphere.h object.h linalg.h material.h texture.h seq.h
sphere.o: gpuProgram.h main.h scene.h light.h eye.h photonmap.h priority.h
sphere.o: rtWindow.h arcballWindow.h window.h
texture.o: headers.h texture.h seq.h linalg.h
triangle.o: headers.h triangle.h object.h linalg.h material.h texture.h seq.h
triangle.o: gpuProgram.h vertex.h main.h scene.h light.h eye.h photonmap.h
triangle.o: priority.h rtWindow.h arcballWindow.h window.h
vertex.o: headers.h vertex.h linalg.h main.h seq.h scene.h object.h
vertex.o: material.h texture.h gpuProgram.h light.h eye.h photonmap.h
vertex.o: priority.h rtWindow.h arcballWindow.h window.h
wavefront.o: headers.h gpuProgram.h linalg.h wavefront.h seq.h shadeMode.h
wavefrontobj.o: headers.h wavefrontobj.h object.h linalg.h material.h
wavefrontobj.o: texture.h seq.h gpuProgram.h wavefront.h shadeMode.h
window.o: headers.h window.h linalg.h
