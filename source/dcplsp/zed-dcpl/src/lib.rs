use zed_extension_api::{self as zed, Command, LanguageServerId, Result, Worktree};

const DCPLSP_PATH: &str =
    "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/build/Release/dcplsp.exe";

struct DcplExtension;

impl zed::Extension for DcplExtension {
    fn new() -> Self {
        Self
    }

    fn language_server_command(
        &mut self,
        _language_server_id: &LanguageServerId,
        _worktree: &Worktree,
    ) -> Result<Command> {
        Ok(Command {
            command: DCPLSP_PATH.into(),
            args: vec![],
            env: vec![],
        })
    }
}

zed::register_extension!(DcplExtension);
