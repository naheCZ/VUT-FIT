/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id:$
 */

#include "student.h"
#include "transform.h"
#include "fragment.h"

#include <memory.h>
#include <math.h>

/*****************************************************************************
 * Globalni promenne a konstanty
 */

/* Typ/ID rendereru (nemenit) */
const int           STUDENT_RENDERER = 1;
float pocet = 0.0f;

const S_Material    MAT_WHITE_AMBIENT  = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    MAT_WHITE_DIFFUSE  = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    MAT_WHITE_SPECULAR = { 1.0, 1.0, 1.0, 1.0 };


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej
 */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renderer->base.type = STUDENT_RENDERER;
    renInit(&renderer->base);

    /* nastaveni ukazatelu na upravene funkce */
    /* napr. renderer->base.releaseFunc = studrenRelease; */
    /* ??? */
	renderer->base.releaseFunc = studrenRelease;
	renderer->base.projectTriangleFunc = studrenProjectTriangle;

    /* inicializace nove pridanych casti */
    /* ??? */
	renderer->textura = (S_RGBA *)malloc(sizeof(S_RGBA));
	IZG_CHECK(renderer, "Cannot allocate enough memory");
	
	renderer->textura = loadBitmap(TEXTURE_FILENAME, &renderer->sirka, &renderer->vyska);

    return (S_Renderer *)renderer;
}

/*****************************************************************************
 * Funkce korektne zrusi renderer a uvolni pamet
 */

void studrenRelease(S_Renderer **ppRenderer)
{
    S_StudentRenderer * renderer;

    if( ppRenderer && *ppRenderer )
    {
        /* ukazatel na studentsky renderer */
        renderer = (S_StudentRenderer *)(*ppRenderer);

        /* pripadne uvolneni pameti */
        /* ??? */
		free(renderer->textura);
        
        /* fce default rendereru */
        renRelease(ppRenderer);
    }
}

/******************************************************************************
 * Nova fce pro rasterizaci trojuhelniku s podporou texturovani
 * Upravte tak, aby se trojuhelnik kreslil s texturami
 * (doplnte i potrebne parametry funkce - texturovaci souradnice, ...)
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
 */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3,
						 S_Coords *t1, S_Coords *t2, S_Coords *t3,
						 double h1, double h2, double h3
                         )
{
    /* zaklad fce zkopirujte z render.c */
    /* ??? */
	int         minx, miny, maxx, maxy;
    int         a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int         s1, s2, s3;
    int         x, y, e1, e2, e3;
    double      alpha, beta, gamma, w1, w2, w3, u, v, z, podil;
    S_RGBA      col1, col2, col3, color, barva;

    IZG_ASSERT(pRenderer && v1 && v2 && v3 && n1 && n2 && n3);

    /* vypocet barev ve vrcholech */
    col1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);
    col2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2);
    col3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);

    /* obalka trojuhleniku */
    minx = MIN(x1, MIN(x2, x3));
    maxx = MAX(x1, MAX(x2, x3));
    miny = MIN(y1, MIN(y2, y3));
    maxy = MAX(y1, MAX(y2, y3));

    /* oriznuti podle rozmeru okna */
    miny = MAX(miny, 0);
    maxy = MIN(maxy, pRenderer->frame_h - 1);
    minx = MAX(minx, 0);
    maxx = MIN(maxx, pRenderer->frame_w - 1);

    /* Pineduv alg. rasterizace troj.
       hranova fce je obecna rovnice primky Ax + By + C = 0
       primku prochazejici body (x1, y1) a (x2, y2) urcime jako
       (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0 */

    /* normala primek - vektor kolmy k vektoru mezi dvema vrcholy, tedy (-dy, dx) */
    a1 = y1 - y2;
    a2 = y2 - y3;
    a3 = y3 - y1;
    b1 = x2 - x1;
    b2 = x3 - x2;
    b3 = x1 - x3;

    /* koeficient C */
    c1 = x1 * y2 - x2 * y1;
    c2 = x2 * y3 - x3 * y2;
    c3 = x3 * y1 - x1 * y3;

    /* vypocet hranove fce (vzdalenost od primky) pro protejsi body */
    s1 = a1 * x3 + b1 * y3 + c1;
    s2 = a2 * x1 + b2 * y1 + c2;
    s3 = a3 * x2 + b3 * y2 + c3;

    if ( !s1 || !s2 || !s3 )
    {
        return;
    }

    /* normalizace, aby vzdalenost od primky byla kladna uvnitr trojuhelniku */
    if( s1 < 0 )
    {
        a1 *= -1;
        b1 *= -1;
        c1 *= -1;
    }
    if( s2 < 0 )
    {
        a2 *= -1;
        b2 *= -1;
        c2 *= -1;
    }
    if( s3 < 0 )
    {
        a3 *= -1;
        b3 *= -1;
        c3 *= -1;
    }

    /* koeficienty pro barycentricke souradnice */
    alpha = 1.0 / ABS(s2);
    beta = 1.0 / ABS(s3);
    gamma = 1.0 / ABS(s1);

    /* vyplnovani... */
    for( y = miny; y <= maxy; ++y )
    {
        /* inicilizace hranove fce v bode (minx, y) */
        e1 = a1 * minx + b1 * y + c1;
        e2 = a2 * minx + b2 * y + c2;
        e3 = a3 * minx + b3 * y + c3;

        for( x = minx; x <= maxx; ++x )
        {
            if( e1 >= 0 && e2 >= 0 && e3 >= 0 )
            {
                /* interpolace pomoci barycentrickych souradnic
                   e1, e2, e3 je aktualni vzdalenost bodu (x, y) od primek */
                w1 = alpha * e2;
                w2 = beta * e3;
                w3 = gamma * e1;

                /* interpolace z-souradnice */
                z = w1 * v1->z + w2 * v2->z + w3 * v3->z;
				//u = w1 * t1->x + w2 * t2->x + w3 * t3->x;
				//v = w1 * t1->y + w2 * t2->y + w3 * t3->y;
				podil = w1 * (1 / h1) + w2 * (1 / h2) + w3 * (1 / h3);				
				u = (w1 * (t1->x / h1) + w2 * (t2->x / h2) + w3 * (t3->x / h3)) / podil;
				v = (w1 * (t1->y / h1) + w2 * (t2->y / h2) + w3 * (t3->y / h3)) / podil;

				//Ziskani barvy z textury
				barva = studrenTextureValue((S_StudentRenderer *) pRenderer, u, v);
              
                /* interpolace barvy */
                color.red = ROUND2BYTE(w1 * col1.red + w2 * col2.red + w3 * col3.red);
                color.green = ROUND2BYTE(w1 * col1.green + w2 * col2.green + w3 * col3.green);
                color.blue = ROUND2BYTE(w1 * col1.blue + w2 * col2.blue + w3 * col3.blue);
                color.alpha = 255;

                //modulace barvy
				color.red = (color.red * barva.red) / color.alpha;
				color.green = (color.green * barva.green) / color.alpha;
				color.blue = (color.blue * barva.blue) / color.alpha;
				
				/* vykresleni bodu */
                if( z < DEPTH(pRenderer, x, y) )
                {
                    PIXEL(pRenderer, x, y) = color;
                    DEPTH(pRenderer, x, y) = z;
                }
            }

            /* hranova fce o pixel vedle */
            e1 += a1;
            e2 += a2;
            e3 += a3;
        }
    }
}

/******************************************************************************
 * Vykresli i-ty trojuhelnik n-teho klicoveho snimku modelu
 * pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * Upravte tak, aby se model vykreslil interpolovane dle parametru n
 * (cela cast n udava klicovy snimek, desetinna cast n parametr interpolace
 * mezi snimkem n a n + 1)
 * i - index trojuhelniku
 * n - index klicoveho snimku (float pro pozdejsi interpolaci mezi snimky)
 */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i, float n)
{
    /* zaklad fce zkopirujte z render.c */
    /* ??? */
	S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci */
    S_Coords    naa, nbb, ncc;          /* normaly ve vrcholech po transformaci */
    S_Coords    nn;                     /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;
    int         vertexOffset, normalOffset, vertexOffsetDalsi, normalOffsetDalsi; /* offset pro vrcholy a normalove vektory trojuhelniku */
    S_Coords    *i0, *i1, *i2, *id0, *id1, *id2, *idn0, *idn1, ik0, ik1, ik2, ikn; /* indexy vrcholu a normaly pro i-ty trojuhelnik n-teho snimku */
	S_Coords	*il0, *il1, *il2, *ib0, *ib1, *ib2, ic0, ic1, ic2;
	int			in, idn;
	double		h1, h2, h3, rozdil;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles) && n >= 0 );

    /* z modelu si vytahneme i-ty trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* ziskame offset pro vrcholy n-teho snimku */
	vertexOffset = (((int) n) % pModel->frames) * pModel->verticesPerFrame;
	vertexOffsetDalsi = (((int) n+1) % pModel->frames) * pModel->verticesPerFrame;

    /* ziskame offset pro normaly trojuhelniku n-teho snimku */
    normalOffset = (((int) n) % pModel->frames) * pModel->triangles->size;
	normalOffsetDalsi = (((int) n+1) % pModel->frames) * pModel->triangles->size;
	
    /* indexy vrcholu pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    i0 = cvecGetPtr(pModel->vertices, triangle->v[0] + vertexOffset);
    i1 = cvecGetPtr(pModel->vertices, triangle->v[1] + vertexOffset);
    i2 = cvecGetPtr(pModel->vertices, triangle->v[2] + vertexOffset);

	id0 = cvecGetPtr(pModel->vertices, triangle->v[0] + vertexOffsetDalsi);
    id1 = cvecGetPtr(pModel->vertices, triangle->v[1] + vertexOffsetDalsi);
    id2 = cvecGetPtr(pModel->vertices, triangle->v[2] + vertexOffsetDalsi);

	il0 = cvecGetPtr(pModel->normals, triangle->v[0] + vertexOffset);
    il1 = cvecGetPtr(pModel->normals, triangle->v[1] + vertexOffset);
    il2 = cvecGetPtr(pModel->normals, triangle->v[2] + vertexOffset);

	ib0 = cvecGetPtr(pModel->normals, triangle->v[0] + vertexOffsetDalsi);
    ib1 = cvecGetPtr(pModel->normals, triangle->v[1] + vertexOffsetDalsi);
    ib2 = cvecGetPtr(pModel->normals, triangle->v[2] + vertexOffsetDalsi);

    /* index normaloveho vektoru pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    in = triangle->n + normalOffset;
	
	idn = triangle->n + normalOffsetDalsi;

    //interpolace
	rozdil = n - (int)n;

	ik0.x = i0->x + (id0->x - i0->x) * rozdil;
	ik1.x = i1->x + (id1->x - i1->x) * rozdil;
	ik2.x = i2->x + (id2->x - i2->x) * rozdil;

	ik0.y = i0->y + (id0->y - i0->y) * rozdil;
	ik1.y = i1->y + (id1->y - i1->y) * rozdil;
	ik2.y = i2->y + (id2->y - i2->y) * rozdil;

	ik0.z = i0->z + (id0->z - i0->z) * rozdil;
	ik1.z = i1->z + (id1->z - i1->z) * rozdil;
	ik2.z = i2->z + (id2->z - i2->z) * rozdil;

	ic0.x = il0->x + (ib0->x - il0->x) * rozdil;
	ic1.x = il1->x + (ib1->x - il1->x) * rozdil;
	ic2.x = il2->x + (ib2->x - il2->x) * rozdil;

	ic0.y = il0->y + (ib0->y - il0->y) * rozdil;
	ic1.y = il1->y + (ib1->y - il1->y) * rozdil;
	ic2.y = il2->y + (ib2->y - il2->y) * rozdil;

	ic0.z = il0->z + (ib0->z - il0->z) * rozdil;
	ic1.z = il1->z + (ib1->z - il1->z) * rozdil;
	ic2.z = il2->z + (ib2->z - il2->z) * rozdil;

	idn0 = cvecGetPtr(pModel->trinormals, in);
	idn1 = cvecGetPtr(pModel->trinormals, idn);
	
	ikn.x = idn0->x + (idn1->x - idn0->x) * rozdil;
	ikn.y = idn0->y + (idn1->y - idn0->y) * rozdil;
	ikn.z = idn0->z + (idn1->z - idn0->z) * rozdil;

	
	/* transformace vrcholu matici model */
    trTransformVertex(&aa, &ik0);
    trTransformVertex(&bb, &ik1);
    trTransformVertex(&cc, &ik2);

    /* promitneme vrcholy trojuhelniku na obrazovku */
    h1 = trProjectVertex(&u1, &v1, &aa);
    h2 = trProjectVertex(&u2, &v2, &bb);
    h3 = trProjectVertex(&u3, &v3, &cc);

    /* pro osvetlovaci model transformujeme take normaly ve vrcholech */
    trTransformVector(&naa, &ic0);
    trTransformVector(&nbb, &ic1);
    trTransformVector(&ncc, &ic2);

    /* normalizace normal */
    coordsNormalize(&naa);
    coordsNormalize(&nbb);
    coordsNormalize(&ncc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, &ikn);
    
    /* normalizace normaly */
    coordsNormalize(&nn);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }
    /* rasterizace trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc,
                    &naa, &nbb, &ncc,
                    u1, v1, u2, v2, u3, v3,
					&triangle->t[0], &triangle->t[1], &triangle->t[2],
					h1, h2, h3
                    );
}

/******************************************************************************
* Vraci hodnotu v aktualne nastavene texture na zadanych
* texturovacich souradnicich u, v
* Pro urceni hodnoty pouziva bilinearni interpolaci
* Pro otestovani vraci ve vychozim stavu barevnou sachovnici dle uv souradnic
* u, v - texturovaci souradnice v intervalu 0..1, ktery odpovida sirce/vysce textury
*/

S_RGBA studrenTextureValue( S_StudentRenderer * pRenderer, double u, double v )
{
    /* ??? */
    //unsigned char c = ROUND2BYTE( ( ( fmod( u * 10.0, 1.0 ) > 0.5 ) ^ ( fmod( v * 10.0, 1.0 ) < 0.5 ) ) * 255 );
	
	int				x, y, x1, y1,s ,vv;
	double			r, g, b, r1, g1, b1, r2, g2, b2;
	double			u_ratio, v_ratio, u_opposite, v_opposite;
	unsigned char	red, green, blue;
	S_RGBA			q11, q12, q21, q22;
	
	s = pRenderer->sirka;
	vv = pRenderer->vyska;
	u = u * ((double)s - 0.5);
	v = v * ((double)vv - 0.5);
	x = (int)u;
	y = (int)v;
	x1 = x + 1;
	y1 = y + 1;
	u_ratio = u - x;
	v_ratio = v - y;
	u_opposite = 1 - u_ratio;
	v_opposite = 1 - v_ratio;

	q11 = pRenderer->textura[s*x + y];
	q12 = pRenderer->textura[s*x + y1];
	q21 = pRenderer->textura[s*x+1 + y];
	q22 = pRenderer->textura[s*x+1 + y1];

	r1 = (q11.red * u_opposite + q21.red * u_ratio) * v_opposite;
	r2 = (q12.red * u_opposite + q22.red * u_ratio) * v_ratio;
	r = r1 + r2;

	g1 = (q11.green * u_opposite + q21.green * u_ratio) * v_opposite;
	g2 = (q12.green * u_opposite + q22.green * u_ratio) * v_ratio;
	g = g1 + g2;

	b1 = (q11.blue * u_opposite + q21.blue * u_ratio) * v_opposite;
	b2 = (q12.blue * u_opposite + q22.blue * u_ratio) * v_ratio;
	b = b1 + b2;

	red = ROUND2BYTE(r);
	green = ROUND2BYTE(g);
	blue = ROUND2BYTE(b);
	
	return makeColor(red, green, blue);
	

	//return makeColor( c, 255 - c, 0 );
}

/******************************************************************************
 ******************************************************************************
 * Funkce pro vyrenderovani sceny, tj. vykresleni modelu
 * Upravte tak, aby se model vykreslil animovane
 * (volani renderModel s aktualizovanym parametrem n)
 */

void renderStudentScene(S_Renderer *pRenderer, S_Model *pModel)
{
    /* zaklad fce zkopirujte z main.c */
    /* ??? */
	/* test existence frame bufferu a modelu */
    IZG_ASSERT(pModel && pRenderer);

    /* nastavit projekcni matici */
    trProjectionPerspective(pRenderer->camera_dist, pRenderer->frame_w, pRenderer->frame_h);

    /* vycistit model matici */
    trLoadIdentity();

    /* nejprve nastavime posuv cele sceny od/ke kamere */
    trTranslate(0.0, 0.0, pRenderer->scene_move_z);

    /* nejprve nastavime posuv cele sceny v rovine XY */
    trTranslate(pRenderer->scene_move_x, pRenderer->scene_move_y, 0.0);

    /* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
    trRotateX(pRenderer->scene_rot_x);
    trRotateY(pRenderer->scene_rot_y);

    /* nastavime material */
    renMatAmbient(pRenderer, &MAT_WHITE_AMBIENT);
    renMatDiffuse(pRenderer, &MAT_WHITE_DIFFUSE);
    renMatSpecular(pRenderer, &MAT_WHITE_SPECULAR);

    /* a vykreslime nas model (ve vychozim stavu kreslime pouze snimek 0) */
    renderModel(pRenderer, pModel, pocet);
}

/* Callback funkce volana pri tiknuti casovace
 * ticks - pocet milisekund od inicializace */
void onTimer( int ticks )
{
    /* uprava parametru pouzivaneho pro vyber klicoveho snimku
     * a pro interpolaci mezi snimky */
    /* ??? */
	pocet += (1.0f/3.03f);
}

/*****************************************************************************
 *****************************************************************************/
