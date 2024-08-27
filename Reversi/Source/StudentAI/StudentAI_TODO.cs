using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.IO.Pipes;
using FullSailAFI.GamePlaying.CoreAI;

namespace FullSailAFI.GamePlaying
{
    public class StudentAI : Behavior
    {
        TreeVisLib treeVisLib;  // lib functions to communicate with TreeVisualization
        bool visualizationFlag = false;  // turn this on to use tree visualization (which you will have to implement via the TreeVisLib API)
                                         // WARNING: Will hang program if the TreeVisualization project is not loaded!

        public StudentAI()
        {
            if (visualizationFlag == true)
            {
                if (treeVisLib == null)  // should always be null, but just in case
                    treeVisLib = TreeVisLib.getTreeVisLib();  // WARNING: Creation of this object will hang if the TreeVisualization project is not loaded!
            }
        }

        //
        // This function starts the look ahead process to find the best move
        // for this player color.
        //
        public ComputerMove Run(int _nextColor, Board _board, int _lookAheadDepth)
        {
            ComputerMove nextMove = GetBestMove(_nextColor, _board, _lookAheadDepth);

            return nextMove;
        }

        //
        // This function uses look ahead to evaluate all valid moves for a
        // given player color and returns the best move it can find. This
        // method will only be called if there is at least one valid move
        // for the player of the designated color.
        //
        private ComputerMove GetBestMove(int color, Board board, int depth)
        {
            //TODO: the lab
            List<ComputerMove> list_Moves = new List<ComputerMove>();
            Board board_Copy = new Board();
            ComputerMove move_Best = null;

            // create moves
            for (int row = 0; row < 8; row++)
            {
                for (int col = 0; col < 8; col++)
                {
                    ComputerMove m = new ComputerMove(0, 0);
                    m.row = row;
                    m.col = col;
                    list_Moves.Add(m);
                }
            }

            // process moves
            foreach (ComputerMove m in list_Moves)
            {
                board_Copy.Copy(board);
                if (board_Copy.IsValidMove(color, m.row, m.col)) // check move is valid
                {
                    board_Copy.MakeMove(color, m.row, m.col); // make move

                    if (board_Copy.IsTerminalState() || depth == 0) // check for end of game
                    {
                        m.rank = Evaluate(board_Copy);
                    }
                    else
                    {
                        if (board_Copy.HasAnyValidMove(-color))
                        {
                            m.rank = Run(-color, board_Copy, depth - 1).rank;
                        }
                        else
                        {
                            m.rank = Run(color, board_Copy, depth - 1).rank;
                        }
                    }

                    if (color == -1) // black
                    {
                        if (move_Best == null || m.rank < move_Best.rank)
                        {
                            move_Best = m;
                        }
                    }
                    else // white
                    {
                        if (move_Best == null || m.rank > move_Best.rank)
                        {
                            move_Best = m;
                        }
                    }
                }
            }

            return move_Best;
        }

        #region Recommended Helper Functions

        private int Evaluate(Board _board)
        {
            //TODO: determine score based on position of pieces
            int val = 0;

            for (int row = 0; row < 8; row++)
            {
                for (int col = 0; col < 8; col++)
                {
                    int color = _board.GetSquareContents(row, col);
                    if ((row == 0 && col == 0) || (row == 7 && col == 0) || (row == 0 && col == 7) || (row == 7 && col == 7))
                    {
                        val += color * 100; // corner
                    }
                    else if (row == 0 || row == 7 || col == 0 || col == 7)
                    {
                        val += color * 10; // side
                    }
                    else
                    {
                        val += color; // all other squares
                    }
                }
            }

            if (_board.IsTerminalState())
            {
                if (val > 0) // positive total
                {
                    val += 10000;
                }
                else // negative total
                {
                    val -= 10000;
                }
            }

            return val;
            /*return ExampleAI.MinimaxAFI.EvaluateTest(_board);*/ // TEST WITH THIS FIRST, THEN IMPLEMENT YOUR OWN EVALUATE
        }

        #endregion

    }
}
