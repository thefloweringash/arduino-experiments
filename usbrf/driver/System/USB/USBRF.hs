module System.USB.USBRF (
  isUSBRF, matchUSBRF,
  USBRFRequest(..),
  RemoteButton(..),
  requestNumber,
  sendButton, readButton
) where

import Data.Binary.Get
import Data.Bits
import Data.Maybe
import Data.Word

import qualified Data.ByteString.Char8 as B
import qualified Data.ByteString.Lazy.Char8 as BL
import qualified Data.Text as T

import System.USB

import qualified System.USB.VUSB.Matcher as VM

isUSBRF :: Device -> IO Bool
isUSBRF = VM.run matchUSBRF

matchUSBRF :: VM.Matcher
matchUSBRF = do
  VM.vendorClassDevice
  VM.textualName "cons.org.nz" "usbrf"

data USBRFRequest = RequestEcho
                  | RequestSend
                  deriving (Show)

instance Enum USBRFRequest where
  fromEnum RequestEcho = 0
  fromEnum RequestSend = 1
  toEnum 0 = RequestEcho
  toEnum 1 = RequestSend

requestNumber :: USBRFRequest -> Word8
requestNumber = fromIntegral . fromEnum

data RemoteButton = RemoteButton { remoteID :: Int , remoteButton :: Int}
                  deriving (Show, Eq)

sendButton :: DeviceHandle -> RemoteButton -> IO ()
sendButton dev b = do
  let (c1, c2) = packRemoteButton b
  control dev Vendor ToDevice (requestNumber RequestSend) c1 c2 noTimeout


binaryGetButton :: B.ByteString -> RemoteButton
binaryGetButton b = flip runGet (BL.fromChunks [b]) $ do
    all <- getWord32le
    return $ RemoteButton { remoteID = fromIntegral (all `shiftR` 4 .&. ((1 `shiftL` 21) - 1))
                          , remoteButton = fromIntegral (all .&. 0xf)
                          }

readButton :: DeviceHandle -> IO (RemoteButton, Status)
readButton dev = do
  (d, r) <- readInterrupt dev buttonEP 4 noTimeout
  return (binaryGetButton d, r)
  where buttonEP = EndpointAddress { endpointNumber = 1, transferDirection = In }

packRemoteButton :: RemoteButton -> (Word16, Word16)
packRemoteButton (RemoteButton r b) =
    let all = fromIntegral $ (r `shiftL` 4) .|. b :: Word32
    in (fromIntegral $ all `shiftR` 16, fromIntegral all)
