

   //RGBA8 RenderBuffer, 24 bit depth RenderBuffer, 512x512
   glGenFramebuffers(1, &fb);
   glBindFramebuffer(GL_FRAMEBUFFER, fb);

   //Create and attach a color buffer
   glGenRenderbuffers(1, &color_rb);

   //We must bind color_rb before we call glRenderbufferStorage
   glBindRenderbuffer(GL_RENDERBUFFER, color_rb);

   //The storage format is RGBA8
   glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 512, 512);

   //Attach color buffer to FBO
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                 GL_RENDERBUFFER, color_rb);

   //-------------------------
   glGenRenderbuffers(1, &depth_rb);
   glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

   //-------------------------
   //Attach depth buffer to FBO
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
						     GL_RENDERBUFFER, depth_rb);

   glEnable(GL_DEPTH_TEST);

   //-------------------------
   //and now you can render to the FBO (also called RenderBuffer)
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //-------------------------
   glViewport(0, 0, 512, 512);
   
   //-------------------------
   // Render into frame buffer
   //

   //     …

   // Set up to read from the renderbuffer and draw to window
   // system framebuffer

   glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);

   //----------------
   //Bind 0, which means render to back buffer
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Do the copy

   glBlitFramebuffer(0, 0, 511, 511, 0, 0 , 511, 511,
					GL_COLOR_BUFFER_BIT, GL_NEAREST);

   glutSwapBuffers();



