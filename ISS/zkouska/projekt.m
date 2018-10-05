%%
I=imread('xlogin00.bmp');
%%
H=[-0.5 -0.5 -0.5; -0.5 5.0 -0.5; -0.5 -0.5 -0.5]
I1 = imfilter(I, H);
imwrite(I1, 'step1.bmp');
%%
I2=fliplr(I1);
imwrite(I2, 'step2.bmp');
%%
I3=medfilt2(I2, [5 5]);
imwrite(I3, 'step3.bmp');
%%
H1=[1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49
I4 = imfilter(I3, H1);
imwrite(I4, 'step4.bmp');
%%
I5=fliplr(I4);
chyba=0;
I6=im2double(I);
I7=im2double(I5);
for (x=1:512)
    for (y=1:512);
        chyba = chyba + double(abs(I6(x, y)-I7(x, y)));
    end;
end;
chyba = (chyba/512/512)*255
%%
I8=im2double(I4);
minimum=min(min(I8));
maximum=max(max(I8));
I9=imadjust(I8, [minimum, maximum], [0, 1]);
imwrite(I9, 'step5.bmp');
%%
mean_no_hist=mean2(I4)
std_no_hist=std2(I4)
mean_hist=mean2(I9)
std_hist=std2(I9)
%%
a = 0;
b = 255;
N = 2;
I10=double(I9);
I11=zeros(512, 512);
for (i=1:512)
    for (j=1:512)
        I11(i, j) = round(((2^N)-1)*(I10(i, j)-a)/(b-a))*(b-a)/((2^N)-1)+a;
    end;
end;
I12=uint8(I11);
imwrite(I12, 'step6.bmp');
%%