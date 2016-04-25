import Text.Parsec.Char
import Text.Parsec.Combinator
import Text.Parsec

import qualified Text.Parsec.Token as P
import Text.Parsec.Language (emptyDef)

import Control.Applicative  ((<$>))
import Control.Monad        (void, fail, unless)

import Data.Map
import Data.Maybe

type Program = [Function]

data Function = Function { funcName   :: String
                         , funcArgs   :: [String]
                         , funcInstrs :: Instructions
                         , funcLabels :: Labels
                         }
              deriving Show

type Instructions = [Instruction]

data Instruction = Instruction { instrName  :: String
                               , instrArgs  :: [Atom]
                               }
                 deriving Show

type Labels = Map String Integer

data Atom = AtomConstant Integer
          | AtomVariable String
          deriving Show

data MiVMParserState = MiVMParserState { stateInstrs    :: Instructions
                                       , stateLabels    :: Labels
                                       , stateLabelPos  :: Integer
                                       }
type MiVMParser a = Parsec String MiVMParserState a

emptyMiVMParserState = MiVMParserState { stateInstrs   = []
                                       , stateLabels   = empty
                                       , stateLabelPos = 0
                                       }




lexer = P.makeTokenParser emptyDef { P.commentLine    = "--"
                                   , P.nestedComments = False
                                   , P.opStart        = unexpected "no operators"
                                   }

colon      = P.colon      lexer
commaSep   = P.commaSep   lexer
dot        = P.dot        lexer
identifier = P.identifier lexer
integer    = P.integer    lexer
parens     = P.parens     lexer
symbol     = P.symbol     lexer

atom       = P.lexeme lexer ((AtomConstant <$> integer)
                         <|> (dot >> AtomVariable <$> identifier))





parseInstr :: MiVMParser ()
parseInstr = do
    let iii name argc = do i <- symbol name
                           a <- commaSep atom
                           let argc' = length a
                           unless (argc' == argc) (fail $ name ++ "wrong argument count "
                                                       ++ show argc' ++ ", expected " ++ show argc)
                           return $ Instruction i a

    -- TODO: generate from opcodes.csv?
    i <- choice [ iii "push"  1
                , iii "pop"   0
                , iii "load"  1
                , iii "store" 1
                , iii "halt"  0
                , iii "add"   0
                , iii "mul"   0
                , iii "inc"   0
                , iii "dec"   1
                , iii "jmp"   1
                , iii "jmpz"  1
                , iii "jmpnz" 0
                , iii "lt"    0
                , iii "eq"    0
                ]

     -- TODO: check variable and label validity

    modifyState (\s -> let is = stateInstrs s
                           lp = stateLabelPos s
                        in s { stateInstrs   = is ++ [i]
                             , stateLabelPos = lp + 1
                             }
                )

parseLabel :: MiVMParser ()
parseLabel = do
    l <- identifier
    colon

    modifyState (\s -> let ls = stateLabels s
                           lp = stateLabelPos s
                        in s { stateLabels = insert l lp ls }
                )

parseFunction :: MiVMParser Function
parseFunction = do
    symbol "fun"
    fName <- identifier
    fArgs <- parens (commaSep identifier)
    colon

    setState emptyMiVMParserState

    many1 $ choice [ try parseLabel
                   , try parseInstr
                   ]

    s <- getState

    return Function { funcName   = fName
                    , funcArgs   = fArgs
                    , funcInstrs = stateInstrs s
                    , funcLabels = stateLabels s
                    }

parseProgram :: MiVMParser Program
parseProgram = do
    p <- many1 parseFunction
    eof
    return p

main = do
    input <- readFile "test1.mivms"
    print $ runParser parseProgram emptyMiVMParserState "MiVM" input
