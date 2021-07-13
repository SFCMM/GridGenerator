//
// Created by svenb on 7/12/21.
//

#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

template <int DEBUG_LEVEL>
class GridGenerator {
 public:
  GridGenerator()                     = default;
  ~GridGenerator()                    = default;
  GridGenerator(const GridGenerator&) = delete;
  GridGenerator(GridGenerator&&)      = delete;
  auto operator=(const GridGenerator&) -> GridGenerator& = delete;
  auto operator=(GridGenerator&&) -> GridGenerator& = delete;


  auto run(int argc, char** argv) -> int;

 private:
};


#endif // GRIDGENERATOR_GRIDGENERATOR_H
