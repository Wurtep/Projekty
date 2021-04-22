import numpy as np

## Class representing Conway's Game of Life
#
# The "living space" of cells is represented by numpy array of size n*n
# where 1 means the cell is alive and 0 means the cell is dead.
# The living space is by default generated randomly, but custom starting
# state can be set so we can experiment with all the different starting states.
class GameOfLife():
  
  ## Constructor.
  # @param dimension The array is n*n, where n is dimension.
  def __init__(self, dimension):
    
    self.dimension = dimension
    self.current_state = np.random.randint(2, size=(self.dimension, self.dimension))

  ## Set custom starting state.
  # @param state Numpy array of size n*n.
  def set_state(self, state):
    
    self.current_state = state
  
  ## Representation of the living space.
  # Prints current_state
  def __str__(self):
    
    return str(self.current_state)

  ## Calculates how many living cells are neighboring cell at [row][col].
  # @row Row of living space.
  # @col Column of living space.
  def living_neighbors(self, row, col):

    if row == 0 and col == 0:
      return self.current_state[row][col+1] + \
        self.current_state[row+1][col+1] + self.current_state[row+1][col]
    elif row == 0 and col != self.dimension -1:
      return self.current_state[row][col+1] + \
        self.current_state[row+1][col+1] + self.current_state[row+1][col] + \
        self.current_state[row+1][col-1] + self.current_state[row][col-1] 
    elif row == 0 and col == self.dimension-1:
      return self.current_state[row+1][col] + \
        self.current_state[row+1][col-1] + self.current_state[row][col-1] 
    elif row < self.dimension-1 and col == 0:
      return self.current_state[row][col+1] + \
        self.current_state[row+1][col+1] + self.current_state[row+1][col] + \
        self.current_state[row-1][col] + self.current_state[row-1][col+1]
    elif row < self.dimension-1 and col != self.dimension-1:
      return self.current_state[row][col+1] + \
        self.current_state[row+1][col+1] + self.current_state[row+1][col] + \
        self.current_state[row+1][col-1] + self.current_state[row][col-1] + \
        self.current_state[row-1][col-1] + self.current_state[row-1][col] + \
        self.current_state[row-1][col+1]
    elif row < self.dimension-1 and col == self.dimension-1:
      return self.current_state[row+1][col] + \
        self.current_state[row+1][col-1] + self.current_state[row][col-1] + \
        self.current_state[row-1][col-1] + self.current_state[row-1][col]
    elif row == self.dimension-1 and col == 0:
      return self.current_state[row][col+1] + \
        self.current_state[row-1][col] + self.current_state[row-1][col+1]
    elif row == self.dimension-1 and col == self.dimension-1:
      return self.current_state[row][col-1] + \
      self.current_state[row-1][col-1] + self.current_state[row-1][col]

  ## Starts the "evolution" of the living space.
  # @param generations How many iterations should be done.
  def live(self, generations):
    
    new_arr = np.zeros(shape=(self.dimension, self.dimension), dtype=int)
    print("Starting state:")
    print(self.current_state)
    
    for gen in range(generations):
      for row in range(self.dimension):
        for col in range(self.dimension):
          if self.current_state[row][col] == 1:
            if self.living_neighbors(row, col) in [2, 3]:
              new_arr[row][col] = 1
            else:
              new_arr[row][col] = 0
          if self.current_state[row][col] == 0:
            if self.living_neighbors(row, col) == 3:
              new_arr[row][col] = 1
            else:
              new_arr[row][col] = 0
              
      self.current_state = new_arr.copy()
      print(f"Generation {gen+1}")
      print(self.current_state)


# example of using
cellular = GameOfLife(5)
# set custom state
cellular.set_state(np.array([
  [0,0,0,0,0],
  [0,0,0,0,0],
  [0,1,1,1,0],
  [0,0,0,0,0],
  [0,0,0,0,0],
  ]))
# do five iterations
cellular.live(5)