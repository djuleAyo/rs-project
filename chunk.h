#ifndef CHUNK_H
#define CHUNK_H

#include <GL/glut.h>
#include <vector>
#include <array>

/* we are defining our own class
 * and not using the one from the lib
 *
 * This class is building block of the map and is built by blocks
 * It is needed so terrain changes don't update much larger data struct the map
 * but only the needed chunk
 *
 * Since we are still not sure what will be a member of chunk
 * wether a block enum or block class or whatever implement chunk as
 * template class
 */

enum class BlockType {
  AIR,
  EARTH
};

enum class CoordType {
  WORLD,
  LOCAL
};

template <class T, short dim>
class Chunk
{
public:
  Chunk(int x = 0, int y = 0, int z = 0);

  //we don't need dim getter since it is static and can be accessed by Chunk::dim

  short originX() const;
  short originY() const;
  short originZ() const;

  //ACCESSING BLOCKS

  // lets not seperate 1 coord index in operator [] and still use ()
  // disallow changing. Use only for reading
  const T operator()(int index) const;
  const T operator()(int x, int y, int z, CoordType t) const;

  //VISIBLES

  /* Which blocks are visible is dependent on observation.
   * If we observe a chunk by it self all limitig blocks are visible
   * We wont need this since it's not that important for a map.
   * In a map we observe all chunks as a group.
   * Still we want to be able to see visible blocks per chunk basis for various
   * testings. Summary: the class doesn't have visible vector member but
   * has method that can compute it
   */

  bool isVisibleBlock(int x, int y, int z, CoordType t) const;
  // this is vector of coords blocktype quadruples
  std::vector<short> &visibleBlocks(std::vector<short> &blocks_out, CoordType t);
  // for visible faces return vector of bit flags coord quadruples
  // for easy bit flag manipulation use std14
  void visibleFaces(std::vector<unsigned short> &faces_out);

  //DRAWING

  /* Draw given visible vector.
   * Since we use drawing gl is dependencie
   */
  void drawAll();
  // No vector member so these can be static
  static void drawVisibleBlocks(const std::vector<T> &blocks);
  static void drawVisibleFaces(const std::vector<unsigned short> &faces);

  static float *visibleBlocksToVbo(const std::vector<T> &blocks);
  static float *visibleFacesToVbo();

  //CHANGES
  void setBlock(int index, T b);
  void fill(T b);

private:
  //chunks are made as cubes of same dimensions
  static const int s_dim = dim;
#define VOLUME s_dim * s_dim * s_dim
  std::array<T, VOLUME> p_blocks {};
  const short p_originX;
  const short p_originY;
  const short p_originZ;

};


//since we are using template class we must define here
//still, define outside of the class def so interface is more visible
//or faster but harder to maintain use 3 files to menage a template


///////////////////////////IMPLEMENTATION//////////////////////////////////////



template<class T, short dim>
Chunk<T, dim>::Chunk(int x, int y, int z)
  : p_originX(x),
    p_originY(y),
    p_originZ(z)
{
  //dummy init of chunk set some blocks
  for(int i = 0; i < p_blocks.size(); i++)
    {
      //if(i % 4 == 0)
        p_blocks[i] = BlockType::EARTH;
      /*else
        p_blocks[i] = BlockType::AIR;
        */
    }
}

template<class T, short dim>
short Chunk<T, dim>::originX() const
{
  return p_originX;
}

template<class T, short dim>
short Chunk<T, dim>::originY() const
{
  return p_originY;
}
template<class T, short dim>
short Chunk<T, dim>::originZ() const
{
  return p_originZ;
}

//ACCESSING BLOCKS

// lets not seperate 1 coord index in operator [] and still use ()
// disallow changing. Use only for reading

template <class T, short dim>
const T Chunk<T, dim>::operator()(int index) const
{
  //out of bounds check
  if( index < 0 || index > VOLUME)
    throw "Out of bounds call on chunk";

  return p_blocks[index];
}

template <class T, short dim>
const T Chunk<T, dim>::operator()(int x, int y,int z, CoordType t) const
{
  if(t != CoordType::LOCAL)
    {
      //check if given coords belong to this chunk
      if(x < p_originX || x > p_originX + s_dim
         || y < p_originY || y > p_originY + s_dim
         || z < p_originZ || z > p_originZ + s_dim
         )
        throw "Given world coords are not part of this chunk";
      return p_blocks[x + p_originX
          + (y + p_originY) * s_dim
          + (z + p_originZ) * s_dim * s_dim
          ];
    }

  if(x < 0 || x > s_dim
     || y < 0 || y > s_dim
     || z < 0 || z > s_dim
     )
    throw "Out of bounds call on chunk";
  return p_blocks[x + y * s_dim + z * s_dim * s_dim];
}

//VISIBLES

/* Which blocks are visible is dependent on observation.
 * If we observe a chunk by it self all limitig blocks are visible
 * We wont need this since it's not that important for a map.
 * In a map we observe all chunks as a group.
 * Still we want to be able to see visible blocks per chunk basis for various
 * testings. Summary: the class doesn't have visible vector member but
 * has method that can compute it
 */
template<class T, short dim>
bool Chunk<T, dim>::isVisibleBlock(int x, int y, int z, CoordType t) const
{
  //normalize coords
  if(t == CoordType::WORLD)
    {
      x = x - p_originX;
      y = y - p_originY;
      z = z - p_originZ;
    }
  //all limiting blocks are visible
  if(x == 0 || x == s_dim - 1
     || y == 0 || y == s_dim - 1
     || z == 0 || z == s_dim - 1
     )
    return true;
  //check if any neighbour is AIR - here this is not limiting block so check 4 sides
  if(operator()(x + 1, y, z, CoordType::LOCAL) == BlockType::AIR) return true;
  if(operator()(x - 1, y, z, CoordType::LOCAL) == BlockType::AIR) return true;
  if(operator()(x, y + 1, z, CoordType::LOCAL) == BlockType::AIR) return true;
  if(operator()(x, y - 1, z, CoordType::LOCAL) == BlockType::AIR) return true;
  if(operator()(x, y, z + 1, CoordType::LOCAL) == BlockType::AIR) return true;
  if(operator()(x, y, z - 1, CoordType::LOCAL) == BlockType::AIR) return true;

  //since all methods for visible failed
  return false;
}

// this is vector of coords block type quadruples
template<class T, short dim>
std::vector<short> &Chunk<T, dim>::visibleBlocks(std::vector<short> &blocks_out,
                                                 CoordType t)
{
  for(int x = 0; x < s_dim; x++)
    {
      for(int y = 0; y < s_dim; y++)
        {
          for(int z = 0; z < s_dim; z++)
            {
              if(isVisibleBlock(x, y, z, CoordType::LOCAL))
                {
                  if(t == CoordType::LOCAL)
                    {
                      blocks_out.push_back(x);
                      blocks_out.push_back(y);
                      blocks_out.push_back(z);
                      blocks_out.push_back(static_cast<short>(
                        //unoptimal since we know that given block is in bounds
                        //so check that () does is not needed
                        *this(x, y, z, CoordType::LOCAL)
                      ));
                    }
                  else
                    {
                      blocks_out.push_back(x + p_originX);
                      blocks_out.push_back(y + p_originY);
                      blocks_out.push_back(z + p_originZ);
                      blocks_out.push_back(static_cast<short>(
                        //unoptimal since we know that given block is in bounds
                        //so check that () does is not needed
                        *this(x, y, z, CoordType::LOCAL)
                      ));
                    }
                }

            }
        }
    }
}
// for visible faces return vector of bit flags coord quadruples
// for easy bit flag manipulation use std14
template<class T, short dim>
void Chunk<T, dim>::visibleFaces(std::vector<unsigned short> &faces_out)
{

}

//DRAWING

/* Draw given visible vector.
 * Since we use drawing gl is dependencie
 */
template<class T, short dim>
void Chunk<T, dim>::drawAll()
{
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glColor3f(1.f, 0.f, 0.f);

  for(int i = 0; i < p_blocks.size(); i++)
    {
      if(p_blocks[i] == BlockType::AIR)
        continue;
      else
        {
          glLoadIdentity();
          glTranslatef( i % 4 , (i % 16) / 4, i / 16);
          glutWireCube(1.);
        }
    }
  glPopMatrix();
}
// No vector member so these can be static
template <class T, short dim>
void Chunk<T, dim>::drawVisibleBlocks(const std::vector<T> &blocks)
{

}
template<class T, short dim>
void Chunk<T, dim>::drawVisibleFaces(const std::vector<unsigned short> &faces)
{

}

//This should be float * for ogl to work with regular array
template <class T, short dim>
float *Chunk<T, dim>::visibleBlocksToVbo(const std::vector<T> &blocks)
{

}
template<class T, short dim>
float *Chunk<T, dim>::visibleFacesToVbo()
{

}

//CHANGES
template <class T, short dim>
void Chunk<T, dim>::setBlock(int index, T b)
{

}

template<class T, short dim>
void Chunk<T, dim>::fill(T b)
{

}

#endif // CHUNK_
