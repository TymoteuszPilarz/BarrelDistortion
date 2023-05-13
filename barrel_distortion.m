clear variables;
close all;

%% Reading image
img = imread('unnamed.png');
img = im2double(img);

red_channel = img(:,:,1);
green_channel = img(:,:,2);
blue_channel = img(:,:,3);

[height, width, channels] = size(img);

%% Adding grid
step = 32;

% % Horizontal lines
% for i = step : step : height - step
%     red_channel(i - 1 : i + 1, 1 : width) = 255 * ones(3, width);
%     green_channel(i - 1 : i + 1, 1 : width) = 255 * ones(3, width);
%     blue_channel(i - 1 : i + 1, 1 : width) = 255 * ones(3, width);
% end
% 
% % Vertical lines
% for i = step : step : width - step
%     red_channel(1: height, i - 1 : i + 1) = 255 * ones(height, 3);
%     green_channel(1 : height, i - 1 : i + 1) = 255 * ones(height, 3);
%     blue_channel(1 : height, i - 1 : i + 1) = 255 * ones(height, 3);
% end

%% Adding example barrel distortion
a = [1.06, -0.0002, 0.000005];  % distortion coefficients
x = 1 : width;
y = 1 : height;
center_x = width / 2 + 0.5;
center_y = height / 2 + 0.5;

[X,Y] = meshgrid(x, y);
r = sqrt((X-center_x).^2 + (Y-center_y).^2);
r_distorted = a(1) * r.^1 + a(2) * r.^2 + a(3) * r.^3;
rn_distorted = r_distorted ./ r;
red_channel = interp2(red_channel, (X-center_x).*rn_distorted+center_x, (Y-center_y).*rn_distorted+center_y, 'linear');
green_channel = interp2(green_channel, (X-center_x).*rn_distorted+center_x, (Y-center_y).*rn_distorted+center_y, 'linear');
blue_channel = interp2(blue_channel, (X-center_x).*rn_distorted+center_x, (Y-center_y).*rn_distorted+center_y, 'linear');
img_distorted = cat(3, red_channel, green_channel, blue_channel);

imshow(img_distorted); title('Distorted');

%% Distortion estimation
i = step : step : width - step;
j = step : step : height - step;
[I, J] = meshgrid(i, j);
r = sqrt((I - center_y).^2 + (J - center_x));
r_distorted = a(1) * r + a(2) * r.^2 + a(3) * r.^3;
r = sort(r(:));
r_distorted = sort(r_distorted(:));
a_estimated = polyfit(r, r_distorted, 3);

%% Calculating r = f(r_distorted)
if (width > height)
    max_dim = width;
else
    max_dim = height;
end
r = 0 : max_dim / 2;
r_distorted = polyval(a_estimated, r);
ainv = polyfit(r_distorted, r, 3);

%% Distortion Correction
[X,Y] = meshgrid(x, y);
r_distorted = sqrt((X-center_x).^2 + (Y-center_y).^2);
Rr = real(polyval(ainv, r_distorted));
rn_distorted = Rr./r_distorted;

red_channel = img_distorted(:,:,1);
green_channel = img_distorted(:,:,2);
blue_channel = img_distorted(:,:,3);

red_channel = interp2(red_channel, (X-center_x).*rn_distorted+center_x, (Y-center_y).*rn_distorted+center_y, 'linear');
green_channel = interp2(green_channel, (X-center_x).*rn_distorted+center_x, (Y-center_y).*rn_distorted+center_y, 'linear');
blue_channel = interp2(blue_channel, (X-center_x).*rn_distorted+center_x, (Y-center_y).*rn_distorted+center_y, 'linear');
img_corrected = cat(3, red_channel, green_channel, blue_channel);

figure;
imshow(img_corrected); title('Corrected');
