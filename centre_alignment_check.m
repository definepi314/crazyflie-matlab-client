    
%specs for array X= X(row, coloumn)
    clc;
    close all;
    clear all;
    cam=webcam('USB_Camera');
    set(cam,'Resolution','1920x1080');
    load('cameraparameters.mat');
    im=snapshot(cam);
    im=imcrop(im,[400 470 900 300]);%  - top right -
    im = undistortImage(im, cameraParams);
    im_gr=rgb2gray(im);
    l_th = graythresh(im_gr);
    crescent_moon_image = im2bw(im_gr, 0.245);% crescent moon im_bw
    full_moon_image = im2bw(im_gr, 0.42);% whole circle im_bw1
    blackout_image = im2bw(im_gr, 0.9);%black screen
    intermediate_image = ~(~full_moon_image + ~crescent_moon_image);
    %imshow(intermediate_image);
    %imshow(crescent_moon_image);
    %imshow(full_moon_image);
    blackout_1 = ~bwareaopen(~blackout_image, 80);
    blackout_2 = bwareaopen(blackout_1, 200);
    s_b=regionprops(~blackout_2,'Centroid');
    pluto_image = intermediate_image + ~crescent_moon_image
    %imshow(pluto_image);
    final_pluto_image = ~bwareaopen(~pluto_image, 80);
    final_full_moon_image = ~bwareaopen(~full_moon_image, 80);
    pluto = bwareaopen(final_pluto_image, 200);
    full_moon = bwareaopen(final_full_moon_image, 200);
    s=regionprops(~pluto,'Centroid');
    s_1=regionprops(~full_moon,'Centroid');
    a=regionprops(~pluto,'Area');
    full_moon = [cat(1,s_1.Centroid)];
    pluto = [cat(1,s.Centroid)];
    centre = [cat(1,s_b.Centroid)];
    h=size(s,1);
    l=size(s_1,1);
    difx = centre(1) - full_moon(1);
    dify = centre(2) - full_moon(2);
    theta = atan2(dify,difx);
    theta = theta*180/pi
    headingangle = atan2((-pluto(2) + full_moon(2)),(-pluto(1) + full_moon(1)));
    headingangle = headingangle*180/3.14
    distance_to_goal = norm(centre - full_moon); 
%  if (theta>0)
%      theta=theta-180;
%  else
%      theta=theta+180;
%  end
%  
%  error=theta-headingangle
%  theta 
%  headingangle
    
    imshow(im); hold on;
%     plot(1,1,'bo');
%     plot(200,60,'bo');
%     plot(200,90,'bo');
%     plot(200,110,'bo');
%     plot(180,60,'bo');
%     plot(240,60,'bo');
%     plot(280,60,'bo');
%     plot(900,300,'bo');
   for x = 1: numel(s)
      plot(s(x).Centroid(1),s(x).Centroid(2),'r*');
      plot(s_1(x).Centroid(1),s_1(x).Centroid(2),'ro');
      plot(s_b(x).Centroid(1),s_b(x).Centroid(2),'b*');
    end
    
        