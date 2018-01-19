//=================================================================================
//  slicSupervoxelMex.c
//
//
//  AUTORIGHTS
//  Copyright (C) 2015 Ecole Polytechnique Federale de Lausanne (EPFL), Switzerland.
//
//  Created by Radhakrishna Achanta on 12/01/15.
//=================================================================================
/*Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met
 
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of EPFL nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "slicsupervoxel.h"

//=================================================================================
//  rgbtolab
//=================================================================================
void rgbtolab(unsigned char* rin, unsigned char* gin, unsigned char* bin, int sz, double* lout, double* aout, double* bout)
{
    int i; int sR, sG, sB;
    double R,G,B;
    double X,Y,Z;
    double r, g, b;
    const double epsilon = 0.008856;	//actual CIE standard
    const double kappa   = 903.3;		//actual CIE standard
    
    const double Xr = 0.950456;	//reference white
    const double Yr = 1.0;		//reference white
    const double Zr = 1.088754;	//reference white
    double xr,yr,zr;
    double fx, fy, fz;
    double lval,aval,bval;
    
    for(i = 0; i < sz; i++)
    {
        sR = rin[i]; sG = gin[i]; sB = bin[i];
        R = sR/255.0;
        G = sG/255.0;
        B = sB/255.0;
        
        if(R <= 0.04045)	r = R/12.92;
        else				r = pow((R+0.055)/1.055,2.4);
        if(G <= 0.04045)	g = G/12.92;
        else				g = pow((G+0.055)/1.055,2.4);
        if(B <= 0.04045)	b = B/12.92;
        else				b = pow((B+0.055)/1.055,2.4);
        
        X = r*0.4124564 + g*0.3575761 + b*0.1804375;
        Y = r*0.2126729 + g*0.7151522 + b*0.0721750;
        Z = r*0.0193339 + g*0.1191920 + b*0.9503041;
        
        //------------------------
        // XYZ to LAB conversion
        //------------------------
        xr = X/Xr;
        yr = Y/Yr;
        zr = Z/Zr;
        
        if(xr > epsilon)	fx = pow(xr, 1.0/3.0);
        else				fx = (kappa*xr + 16.0)/116.0;
        if(yr > epsilon)	fy = pow(yr, 1.0/3.0);
        else				fy = (kappa*yr + 16.0)/116.0;
        if(zr > epsilon)	fz = pow(zr, 1.0/3.0);
        else				fz = (kappa*zr + 16.0)/116.0;
        
        lval = 116.0*fy-16.0;
        aval = 500.0*(fx-fy);
        bval = 200.0*(fy-fz);
        
        lout[i] = lval; aout[i] = aval; bout[i] = bval;
    }
}

//=================================================================================
///  getVoxelSeeds
///
/// Works the same way for color and grayscale cases.
//=================================================================================
void getVoxelSeeds(int width, int height, int depth, int numReqdSupervoxels, double step, int* seedIndices, int* numseeds)
{
#if 0//older code, less precise number of seeds
    const bool hexgrid = false;
    int n;
    int xstrips, ystrips, zstrips;
    int xerr, yerr,zerr;
    double xerrperstrip,yerrperstrip,zerrperstrip;
    int xoff,yoff,zoff;
    int x,y,z;
    int xe,ye,ze;
    int seedx,seedy,seedz;
    int i;
    int sz2 = width*height;
    double STEP = step;

    xstrips = (0.5+(double)(width)/(double)(STEP));     if(xstrips <= 0) xstrips = 1;
    ystrips = (0.5+(double)(height)/(double)(STEP));    if(ystrips <= 0) ystrips = 1;
    zstrips = (0.5+(double)(depth)/(double)(STEP));     if(zstrips <= 0) zstrips = 1;
    
    xerr = width  - STEP*xstrips;if(xerr < 0){xerr = width - STEP*xstrips;}
    yerr = height - STEP*ystrips;if(yerr < 0){yerr = height- STEP*ystrips;}
    zerr = depth  - STEP*zstrips;if(zerr < 0){zerr = depth - STEP*zstrips;}
    
    xerrperstrip = (double)(xerr)/(double)(xstrips);
    yerrperstrip = (double)(yerr)/(double)(ystrips);
    zerrperstrip = (double)(zerr)/(double)(zstrips);

    xoff = STEP/2; if(STEP >= width) xoff = width/2;
    yoff = STEP/2; if(STEP >= height)yoff = height/2;
    zoff = STEP/2; if(STEP >= depth) zoff = depth/2;
    
    n = 0;
    for( z = 0; z < zstrips; z++ )
    {
        ze = z*zerrperstrip;
        for( y = 0; y < ystrips; y++ )
        {
            ye = y*yerrperstrip;
            for( x = 0; x < xstrips; x++ )
            {
                xe = x*xerrperstrip;
                seedx = (x*STEP+xoff+xe);
                seedy = (y*STEP+yoff+ye);
                seedz = (z*STEP+zoff+ze);
                i = seedz*sz2 + seedy*width + seedx;
                seedIndices[n] = i;
                n++;
            }
        }
    }
    *numseeds = n;
    
#else
    int nxsteps,nysteps,nzsteps;
    double xstep,ystep,zstep;
    double xoff,yoff,zoff;
    int n, x, y, z, i;
    int seedx,seedy,seedz;
    int numcreatedseeds;
    int numdiff;
    int dimind;
    int dims[3];
    const int sz2 = width*height;
    const int sz3 = width*height*depth;
    double supervoxelvolume = (double)sz3/(double)numReqdSupervoxels;
    
    xstep = ystep = zstep = step;
    
    nxsteps = (double)width /(double)step; if(nxsteps <= 0) nxsteps = 1;
    nysteps = (double)height/(double)step; if(nysteps <= 0) nysteps = 1;
    nzsteps = (double)depth /(double)step; if(nzsteps <= 0) nzsteps = 1;
    
    dims[0] = nxsteps;
    dims[1] = nysteps;
    dims[2] = nzsteps;
    numcreatedseeds = dims[0]*dims[1]*dims[2];
    dimind = 0;
    while(numcreatedseeds < numReqdSupervoxels)
    {
        dims[dimind]++;
        numcreatedseeds = dims[0]*dims[1]*dims[2];
        dimind++; if(dimind >= 3) dimind = 0;
    }
    numdiff = abs(numcreatedseeds-numReqdSupervoxels);
    dimind = 0;
    while(numcreatedseeds > numReqdSupervoxels)
    {
        if(dims[dimind] > 1) dims[dimind]--;
        numcreatedseeds = dims[0]*dims[1]*dims[2];
        if(abs(numcreatedseeds-numReqdSupervoxels) > numdiff)
        {
            dims[dimind]++;
            break;
        }
        dimind++; if(dimind >= 3) dimind = 0;
    }
    nxsteps = dims[0];
    nysteps = dims[1];
    nzsteps = dims[2];
    
    xstep = (double)width/(double)nxsteps;
    ystep = (double)height/(double)nysteps;
    zstep = (double)depth/(double)nzsteps;
    
    if(xstep >= width) xstep = 0;
    if(ystep >= height)ystep = 0;
    if(zstep >= depth) zstep = 0;
    
    xoff = step/2.0; if(step >= width) xoff = width /2.0;
    yoff = step/2.0; if(step >= height)yoff = height/2.0;
    zoff = step/2.0; if(step >= depth) zoff = depth /2.0;
    
    n = 0;
    for(int z = 0; z < nzsteps; z++)
    {
        seedz = zoff + z*zstep; if(seedz >= depth) seedz = depth-zoff;
        for(int y = 0; y < nysteps; y++)
        {
            seedy = yoff + y*ystep; if(seedy >= height) seedy = height-yoff;
            for(int x = 0; x < nxsteps; x++)
            {
                seedx = xoff + x*xstep; if(seedx >= width) seedx = width-xoff;
                i = seedz*sz2 + seedy*width + seedx;
                seedIndices[n] = i;
                n++;
            }
        }
    }
    *numseeds = n;
#endif
}

//=================================================================================
//  PerformSupervoxelSLIC
//
//	For a color stack.
//=================================================================================
void PerformSupervoxelSLIC(double* lvec, double* avec, double* bvec, double* kseedsl, double* kseedsa, double* kseedsb, double* kseedsx, double* kseedsy, double* kseedsz, int width, int height, int depth, int numseeds, int* klabels, int STEP, double compactness)
{
    int x1, y1, x2, y2, z1, z2;
    double l, a, b;
    double dist;
    double distxyz;
    int itr;
    int n;
    int x,y,z;
    int i;
    int ind;
    int r,c,d;
    int k;
    int sz2 = width*height;
    int sz3 = width*height*depth;
    const int numk = numseeds;
    int offset = STEP;

    double* clustersize = allocate_memory<double>(numk);
    double* inv         = allocate_memory<double>(numk);
    double* sigmal      = allocate_memory<double>(numk);
    double* sigmaa      = allocate_memory<double>(numk);
    double* sigmab      = allocate_memory<double>(numk);
    double* sigmax      = allocate_memory<double>(numk);
    double* sigmay      = allocate_memory<double>(numk);
    double* sigmaz      = allocate_memory<double>(numk);
    double* distvec     = allocate_memory<double>(sz3);
    
    double invwt = 1.0/((STEP/compactness)*(STEP/compactness));
    
    for( itr = 0; itr < 5; itr++ )
    {
        for(i = 0; i < sz3; i++){distvec[i] = DBL_MAX;}
     
        for( n = 0; n < numk; n++ )
        {
            x1 = kseedsx[n]-offset; if(x1 < 0) x1 = 0;
            y1 = kseedsy[n]-offset; if(y1 < 0) y1 = 0;
            z1 = kseedsz[n]-offset; if(z1 < 0) z1 = 0;
            x2 = kseedsx[n]+offset; if(x2 > width)  x2 = width;
            y2 = kseedsy[n]+offset; if(y2 > height) y2 = height;
            z2 = kseedsz[n]+offset; if(z2 > depth)  z2 = depth;
            
            for( z = z1; z < z2; z++ )
            {
                for( y = y1; y < y2; y++ )
                {
                    for( x = x1; x < x2; x++ )
                    {
                        i = z*sz2 + y*width + x;
                        
                        l = lvec[i];
                        a = avec[i];
                        b = bvec[i];
                        
                        dist =			(l - kseedsl[n])*(l - kseedsl[n]) +
                            (a - kseedsa[n])*(a - kseedsa[n]) +
                            (b - kseedsb[n])*(b - kseedsb[n]);
                        
                        distxyz =		(x - kseedsx[n])*(x - kseedsx[n]) +
                            (y - kseedsy[n])*(y - kseedsy[n]) +
                            (z - kseedsz[n])*(z - kseedsz[n]);
                        
                        dist += (distxyz*invwt);
                        
                        if(dist < distvec[i])
                        {
                            distvec[i] = dist;
                            klabels[i]  = n;
                        }
                    }
                }
            }
        }
        //-----------------------------------------------------------------
        // Recalculate the centroid and store in the seed values
        //-----------------------------------------------------------------
        for(k = 0; k < numk; k++)
        {
            sigmal[k] = 0;
            sigmaa[k] = 0;
            sigmab[k] = 0;
            sigmax[k] = 0;
            sigmay[k] = 0;
            sigmaz[k] = 0;
            clustersize[k] = 0;
        }
        
        ind = 0;
        for( d = 0; d < depth; d++)
        {
            for( r = 0; r < height; r++ )
            {
                for( c = 0; c < width; c++ )
                {
                    if(klabels[ind] >= 0 && klabels[ind] < numk)
                    {
                        sigmal[klabels[ind]] += lvec[ind];
                        sigmaa[klabels[ind]] += avec[ind];
                        sigmab[klabels[ind]] += bvec[ind];
                        sigmax[klabels[ind]] += c;
                        sigmay[klabels[ind]] += r;
                        sigmaz[klabels[ind]] += d;
                        clustersize[klabels[ind]] += 1.0;
                    }
                    ind++;
                }
            }
        }
        
        {for( k = 0; k < numk; k++ )
            {
                if( clustersize[k] <= 0 ) clustersize[k] = 1;
                inv[k] = 1.0/clustersize[k];//computing inverse now to avoid division
            }}
                
        {for( k = 0; k < numk; k++ )
            {
                kseedsl[k] = sigmal[k]*inv[k];
                kseedsa[k] = sigmaa[k]*inv[k];
                kseedsb[k] = sigmab[k]*inv[k];
                kseedsx[k] = sigmax[k]*inv[k];
                kseedsy[k] = sigmay[k]*inv[k];
                kseedsz[k] = sigmaz[k]*inv[k];
            }}
    }
    deallocate_memory(sigmal);
    deallocate_memory(sigmaa);
    deallocate_memory(sigmab);
    deallocate_memory(sigmax);
    deallocate_memory(sigmay);
    deallocate_memory(sigmaz);
    deallocate_memory(clustersize);
    deallocate_memory(inv);
    deallocate_memory(distvec);
}

//=================================================================================
//  PerformSupervoxelSLIC_gray
//
//	For a grayscale stack.
//=================================================================================
void PerformSupervoxelSLIC_gray(double* lvec, double* kseedsl, double* kseedsx, double* kseedsy, double* kseedsz, int width, int height, int depth, int numseeds, int* klabels, int STEP, double compactness)
{
    int x1, y1, x2, y2, z1, z2;
    double dist;
    double distxyz;
    int itr;
    int n;
    int x,y,z;
    int i;
    int ind;
    int r,c,d;
    int k;
    int sz2 = width*height;
    int sz3 = width*height*depth;
    const int numk = numseeds;
    int offset = STEP;
    
    double* clustersize = allocate_memory<double>(numk);
    double* inv         = allocate_memory<double>(numk);
    double* sigmal      = allocate_memory<double>(numk);
    double* sigmax      = allocate_memory<double>(numk);
    double* sigmay      = allocate_memory<double>(numk);
    double* sigmaz      = allocate_memory<double>(numk);
    double* distvec     = allocate_memory<double>(sz3);
    double invwt = 1.0/((STEP/compactness)*(STEP/compactness));
    
    for( itr = 0; itr < 5; itr++ )
    {
        for(i = 0; i < sz3; i++){distvec[i] = DBL_MAX;}
        
        for( n = 0; n < numk; n++ )
        {
            x1 = kseedsx[n]-offset; if(x1 < 0) x1 = 0;
            y1 = kseedsy[n]-offset; if(y1 < 0) y1 = 0;
            z1 = kseedsz[n]-offset; if(z1 < 0) z1 = 0;
            x2 = kseedsx[n]+offset; if(x2 > width)  x2 = width;
            y2 = kseedsy[n]+offset; if(y2 > height) y2 = height;
            z2 = kseedsz[n]+offset; if(z2 > depth)  z2 = depth;
            
            for( z = z1; z < z2; z++ )
            {
                for( y = y1; y < y2; y++ )
                {
                    for( x = x1; x < x2; x++ )
                    {
                        i = z*sz2 + y*width + x;
                        
                        dist =			(lvec[i] - kseedsl[n])*(lvec[i] - kseedsl[n]);
                        distxyz =		(x - kseedsx[n])*(x - kseedsx[n]) + (y - kseedsy[n])*(y - kseedsy[n]) + (z - kseedsz[n])*(z - kseedsz[n]);
                        
                        dist += (distxyz*invwt);
                        
                        if(dist < distvec[i])
                        {
                            distvec[i] = dist;
                            klabels[i]  = n;
                        }
                    }
                }
            }
        }
        //-----------------------------------------------------------------
        // Recalculate the centroid and store in the seed values
        //-----------------------------------------------------------------
        for(k = 0; k < numk; k++)
        {
            sigmal[k] = 0;
            sigmax[k] = 0;
            sigmay[k] = 0;
            sigmaz[k] = 0;
            clustersize[k] = 0;
        }
        
        ind = 0;
        for( d = 0; d < depth; d++)
        {
            for( r = 0; r < height; r++ )
            {
                for( c = 0; c < width; c++ )
                {
                    if(klabels[ind] >= 0 && klabels[ind] < numk)
                    {
                        sigmal[klabels[ind]] += lvec[ind];
                        sigmax[klabels[ind]] += c;
                        sigmay[klabels[ind]] += r;
                        sigmaz[klabels[ind]] += d;
                        clustersize[klabels[ind]] += 1.0;
                    }
                    ind++;
                }
            }
        }
        
        {for( k = 0; k < numk; k++ )
            {
                if( clustersize[k] <= 0 ) clustersize[k] = 1;
                inv[k] = 1.0/clustersize[k];//computing inverse now to avoid division
            }}
        
        {for( k = 0; k < numk; k++ )
            {
                kseedsl[k] = sigmal[k]*inv[k];
                kseedsx[k] = sigmax[k]*inv[k];
                kseedsy[k] = sigmay[k]*inv[k];
                kseedsz[k] = sigmaz[k]*inv[k];
            }}
    }
    deallocate_memory(sigmal);
    deallocate_memory(sigmax);
    deallocate_memory(sigmay);
    deallocate_memory(sigmaz);
    deallocate_memory(clustersize);
    deallocate_memory(inv);
    deallocate_memory(distvec);
}

//=================================================================================
//  EnforceSupervoxelConnectivity
//=================================================================================
void EnforceSupervoxelConnectivity(int* labels, int width, int height, int depth, int numSuperpixels, int* nlabels, int* finalNumberOfLabels)
{
    int i,j,k;
    int n,c,count;
    int x,y,z;
    int ind;
    int label;
    const int dx10[10] = {-1,  0,  1,  0, -1,  1,  1, -1,  0, 0};
    const int dy10[10] = { 0, -1,  0,  1, -1, -1,  1,  1,  0, 0};
    const int dz10[10] = { 0,  0,  0,  0,  0,  0,  0,  0, -1, 1};
    const int sz2 = width*height;
    const int sz3 = width*height*depth;
    const int SUPSZ = sz3/numSuperpixels;
    int BUFFSZ = SUPSZ*30; if(BUFFSZ > sz3) BUFFSZ = sz3;
    int MINSUPSZ = SUPSZ >> 3; if(SUPSZ <= 25) MINSUPSZ = 3;
    int* xvec = allocate_memory<int>(BUFFSZ);
    int* yvec = allocate_memory<int>(BUFFSZ);
    int* zvec = allocate_memory<int>(BUFFSZ);
        
    for( i = 0; i < sz3; i++ ) nlabels[i] = -1;
    int oindex = 0;
    int adjlabel = 0;//adjacent label
    label = 0;
    for( i = 0; i < depth; i++ )
    {
        for( j = 0; j < height; j++ )
        {
            for( k = 0; k < width; k++ )
            {
                if( 0 > nlabels[oindex] )
                {
                    nlabels[oindex] = label;
                    //--------------------
                    // Start a new segment
                    //--------------------
                    xvec[0] = k;
                    yvec[0] = j;
                    zvec[0] = i;
                    //-------------------------------------------------------
                    // Quickly find an adjacent label for use later if needed
                    //-------------------------------------------------------
                    {for( n = 0; n < 10; n++ )
                        {
                            x = xvec[0] + dx10[n];
                            y = yvec[0] + dy10[n];
                            z = zvec[0] + dz10[n];
                            if(!(x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth))
                            {
                                int nindex = z*sz2 + y*width + x;
                                if(nlabels[nindex] >= 0) adjlabel = nlabels[nindex];
                            }
                        }}
                    
                    count = 1;
                    for( c = 0; c < count; c++ )
                    {
                        for( n = 0; n < 10; n++ )
                        {
                            x = xvec[c] + dx10[n];
                            y = yvec[c] + dy10[n];
                            z = zvec[c] + dz10[n];
                            if(!(x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth))
                            {
                                int nindex = z*sz2 + y*width + x;
                                int t1 = nlabels[nindex];
                                int t2 = labels[oindex];
                                int t3 = labels[nindex];
                                if( 0 > nlabels[nindex] && labels[oindex] == labels[nindex] )
                                {
                                    xvec[count] = x;
                                    yvec[count] = y;
                                    zvec[count] = z;
                                    nlabels[nindex] = label;
                                    count++;
                                }
                            }
                            
                        }
                    }
                    //-------------------------------------------------------
                    // If segment size is less then a limit, assign an
                    // adjacent label found before, and decrement label count.
                    //-------------------------------------------------------
                    if(count <= MINSUPSZ)//this is the size limit for absorbing a small supervoxel into a big one
                    {
                        for( c = 0; c < count; c++ )
                        {
                            ind = zvec[c]*sz2 + yvec[c]*width + xvec[c];
                            nlabels[ind] = adjlabel;
                        }
                        label--;
                    }
                    label++;
                }
                oindex++;
            }
        }
    }
    *finalNumberOfLabels = label;
    
    deallocate_memory(xvec);
    deallocate_memory(yvec);
    deallocate_memory(zvec);
}

//=================================================================================
//  mexFunction
//
//  Main entry function
//
//  Takes as input
//  - an image stack (color or grayscale) and
//  - the number of supervoxels requried
//
//  Generates output:
//  - supervoxel label volume (same indexing order as input stack)
//  - number of generated supervoxels (which could differ from the input number)
//=================================================================================
bp::tuple svx::run(bn::ndarray stack, int numReqdSupervoxels=200, double compactness=10)
{
    //---------------------------
    // Variable declarations
    //---------------------------
    int width;
    int height;
    int depth;
    int colors;
    int sz2;
    int sz3;
    int sz4;
    int i, ii;
    int x, y, z;
    unsigned char* rin; unsigned char* gin; unsigned char* bin;
    int* klabels;
    int* clabels;
    double* lvec; double* avec; double* bvec;
    int step;
    int* seedIndices;
    int numseeds;
    double* kseedsx;double* kseedsy;double* kseedsz;
    double* kseedsl;double* kseedsa;double* kseedsb;
    int k;
    const long int* dims;//int* dims;
    int outputNumSuperpixels;
    int* outlabels;
    int finalNumberOfLabels;
    unsigned char* imgbytes;
    int testcount;

    //---------------------------
    int numelements   = get_n_elems(stack);
    int numdims = stack.get_nd() ;
    dims  = stack.get_shape();
    imgbytes = getArrayInput<unsigned char>(stack);
    //std::cout << "numdims: " << numdims << std::endl;
    //std::cout << "numelements: " << numelements << std::endl;
    //std::cout << "dims: " << dims << std::endl;
    if(3 == numdims)
    {
        depth = dims[2];
        width = dims[1];
        height = dims[0];
    }
    else if(4 == numdims)
    {
        depth = dims[3];
        colors = dims[2];
        width = dims[1];
        height = dims[0];
    }

    //std::cout << "width: " << width << std::endl;
    //std::cout << "height: " << height << std::endl;
    //std::cout << "depth: " << depth << std::endl;
    //std::cout << "colors: " << colors << std::endl;
    
    sz2 = width*height;
    sz3 = width*height*depth;
    sz4 = width*height*depth*colors;

    //---------------------------
    //---------------------------
    // Allocate memory
    //---------------------------
    klabels = allocate_memory<int>(sz3);//original k-means labels
    clabels = allocate_memory<int>(sz3);//corrected labels after enforcing connectivity
    seedIndices = allocate_memory<int>(sz3);
    
    //---------------------------
    // Find seeds
    //---------------------------
    step = pow((double)(sz3)/(double)(numReqdSupervoxels), 1.0/3.0) + 0.5;//step size is the cube-root of the average area of a supervoxel
    getVoxelSeeds(width,height,depth,numReqdSupervoxels,step,seedIndices,&numseeds);
    kseedsx    = allocate_memory<double>(numseeds);
    kseedsy    = allocate_memory<double>(numseeds);
    kseedsz    = allocate_memory<double>(numseeds);
    kseedsl    = allocate_memory<double>(numseeds);
    
    //---------------------------
    // Perform color conversion if needed.
    //---------------------------
    if(numelements/sz3 == 1)//if it is a grayscale image, copy the values into the l vector
    {
        //std::cout << "Is grayscale" << std::endl;
        //---------------------------
        lvec = allocate_memory<double>(sz3) ;
        //---------------------------
        for(z = 0, ii = 0; z < depth; z++)
        {
            for(x = 0; x < width; x++)//reading data from column-major MATLAB matrics to row-major C matrices (i.e perform transpose)
            {
                for(y = 0; y < height; y++)
                {
                    i = z*sz2 + y*width+x;
                    lvec[i] = imgbytes[ii];
                    ii++;
                }
            }
        }
        for(k = 0; k < numseeds; k++)
        {
            if(seedIndices[k] >= 0)
            {
                kseedsz[k] = (int)(seedIndices[k]/sz2);
                kseedsy[k] = (int)((seedIndices[k]-kseedsz[k]*sz2)/width);
                kseedsx[k] = (int)(seedIndices[k]-kseedsz[k]*sz2)%width;
                kseedsl[k] = lvec[seedIndices[k]];
            }
        }
        //---------------------------
        // Compute superpixels
        //---------------------------
        PerformSupervoxelSLIC_gray(lvec,kseedsl,kseedsx,kseedsy,kseedsz,width,height,depth,numseeds,klabels,step,compactness);
    }
    else//for color image volume
    {
        //std::cout << "Is color" << std::endl;
        //std::cout << "numseeds: " << numseeds << std::endl;
        rin     = allocate_memory<unsigned char>(sz3) ;
        gin     = allocate_memory<unsigned char>(sz3) ;
        bin     = allocate_memory<unsigned char>(sz3) ;
        lvec    = allocate_memory<double>(sz3) ;
        avec    = allocate_memory<double>(sz3) ;
        bvec    = allocate_memory<double>(sz3) ;
        kseedsa    = allocate_memory<double>(numseeds) ;
        kseedsb    = allocate_memory<double>(numseeds) ;
        for(z = 0; z < depth; z++)
        {
            ii = z*sz2*3;
            for(y = 0; y < height; y++)
            {
                for(x = 0; x < width; x++)
                {
                    i = z*sz2 + y*width + x;
                    rin[i] = imgbytes[ii];
                    gin[i] = imgbytes[ii+1];
                    bin[i] = imgbytes[ii+2];
                    ii += 3;
                }
            }
        }
        
        rgbtolab(rin,gin,bin,sz3,lvec,avec,bvec);
        //std::cout << "rin[1000]: " << (int)(rin[1000]) << std::endl;
        //std::cout << "gin[1000]: " << (int)(gin[1000]) << std::endl;
        //std::cout << "bin[1000]: " << (int)(bin[1000]) << std::endl;
        //std::cout << "lvec[1000]" << lvec[1000] << std::endl;
        //std::cout << "avec[1000]" << avec[1000] << std::endl;
        //std::cout << "bvec[1000]" << bvec[1000] << std::endl;
        deallocate_memory(rin);
        deallocate_memory(gin);
        deallocate_memory(bin);
        
        for(k = 0; k < numseeds; k++)
        {
            if(seedIndices[k] >= 0)
            {
                kseedsz[k] = (int)(seedIndices[k]/sz2);
                kseedsy[k] = (int)((seedIndices[k]-kseedsz[k]*sz2)/width);
                kseedsx[k] = (int)(seedIndices[k]-kseedsz[k]*sz2)%width;
                kseedsl[k] = lvec[seedIndices[k]];
                kseedsa[k] = avec[seedIndices[k]];
                kseedsb[k] = bvec[seedIndices[k]];
            }
        }
        //---------------------------
        // Compute superpixels
        //---------------------------
        PerformSupervoxelSLIC(lvec, avec, bvec, kseedsl,kseedsa,kseedsb,kseedsx,kseedsy,kseedsz,width,height,depth,numseeds,klabels,step,compactness);
        //std::cout << "Done PerformSupervoxelSLIC" << std::endl;
    }
    
    
    //---------------------------
    // Enforce connectivity
    //---------------------------
    EnforceSupervoxelConnectivity(klabels,
                                  width,
                                  height,
                                  depth,
                                  numReqdSupervoxels,
                                  clabels,
                                  &finalNumberOfLabels);
    //std::cout << "Done EnforceSupervoxelConnectivity" << std::endl;

    //---------------------------
    // Assign output labels
    //---------------------------
    int ndims[3];
    ndims[0] = height;
    ndims[1] = width;
    ndims[2] = depth;
    //std::cout << "Made ndims array" << std::endl;
    //bn::ndarray labels = bn::empty(bp::make_tuple(height,width,depth),bn::dtype::get_builtin<int>());
    //std::cout << "Made labels empty array" << std::endl;

    //outlabels = getArrayInput<int>(labels);
    bp::tuple shape = bp::make_tuple(height,width,depth);

    bn::ndarray *labels = npWriteStack(width,height,depth, clabels);
    //bn::ndarray labels = toNumpy(width, height, depth, clabels);
    //std::cout << "after from_data" << std::endl;

    //std::cout << "Populated outlabels" << std::endl;
    //std::cout << "finalNumberOfLabels:" << finalNumberOfLabels << std::endl;

    //---------------------------
    // Assign number of labels/seeds
    outputNumSuperpixels = finalNumberOfLabels;
    //std::cout << "Assigned outputNumSuperpixels" << std::endl;
    //std::cout << "outputNumSuperpixels:" << *outputNumSuperpixels << std::endl;
    //------------------------

    //---------------------------
    // Deallocate memory
    //---------------------------
    deallocate_memory(kseedsx);
    deallocate_memory(kseedsy);
    deallocate_memory(kseedsz);
    deallocate_memory(kseedsl);
    if(numelements/sz3 > 1)//if it is a color image volume
    {
        deallocate_memory(kseedsa);
        deallocate_memory(kseedsb);
        deallocate_memory(avec);
        deallocate_memory(bvec);
    }
    deallocate_memory(lvec);
    deallocate_memory(klabels);
    //deallocate_memory(clabels);
    deallocate_memory(seedIndices);

    //std::cout << "Before return" << std::endl;
    return bp::make_tuple(*labels, outputNumSuperpixels);
}
