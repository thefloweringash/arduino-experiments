import Control.Concurrent
import Control.Exception
import Control.Monad

import System.USB
import System.USB.USBRF

doLSUSB :: [Device] -> IO ()
doLSUSB devs = do
  forM_ devs $ \dev -> do
    matched <- isUSBRF dev
    print (dev, matched)

interactive :: DeviceHandle -> IO ()
interactive handle = do
  putStrLn $ "Got device " ++ show handle
  claimInterface handle 0

  forkIO $ forever $ do
    let buttonEP = EndpointAddress { endpointNumber = 1, transferDirection = In }
    print =<< readButton handle

  forever $ do
    putStrLn "get line"
    [remote, button] <- fmap words getLine
    putStrLn "got line"
    r <- sendButton handle $ RemoteButton (read remote) (read button)
    print r

main = do
  ctx <- newCtx
  devs <- getDevices ctx
  probeDevs <- filterM isUSBRF devs
  case probeDevs of
    [] -> doLSUSB devs
    (dev:_) -> withDeviceHandle dev interactive
