-- A subset of the rules for vusb's free IDs. Please extend if
-- necessary. This module should be imported qualified

module System.USB.VUSB.Matcher (
  Matcher,
  run,
  ids,
  textualName,
  vendorClassDevice
)  where

import Control.Monad.Maybe
import Control.Monad.Reader
import Control.Monad.Trans

import Data.Maybe
import Data.Word

import qualified Data.Text as T

import System.USB

runMaybe :: Monad m => Maybe a -> MaybeT m a
runMaybe = MaybeT . return

type Matcher = MaybeT (ReaderT Device IO) ()

run :: Matcher -> Device -> IO Bool
run m d = fmap isJust . flip runReaderT d . runMaybeT $ m

ids :: Word16 -> Word16 -> Matcher
ids vid pid = do
  desc <- asks deviceDesc
  guard $ deviceVendorId  desc == vid
  guard $ deviceProductId desc == pid

vendorClassDevice :: Matcher
vendorClassDevice = ids 5824 1500

textualName :: String -> String -> Matcher
textualName manufacturer product = do
  dev  <- ask
  desc <- asks deviceDesc

  manufacturerIdx <- runMaybe $ deviceManufacturerStrIx desc
  productIdx      <- runMaybe $ deviceProductStrIx      desc

  anyLanguageMatches <- liftIO $ do
    let productText      = T.pack product
        manufacturerText = T.pack manufacturer
    withDeviceHandle dev $ \h -> flip run dev $ do
      langs <- liftIO $ getLanguages h
      msum $ flip map langs $ \lang -> do
        devManufacturer <- liftIO $ getStrDesc h manufacturerIdx lang 255
        guard $ devManufacturer == manufacturerText
        devProduct <- liftIO $ getStrDesc h productIdx lang 255
        guard $ devProduct == productText
        return ()

  guard anyLanguageMatches
